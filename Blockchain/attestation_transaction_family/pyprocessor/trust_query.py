# Copyright 2017 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# -----------------------------------------------------------------------------

import logging
import block_info_functions
import storage_functions
import datetime
import textwrap
import response_TQ_pb2
from sawtooth_sdk.processor.exceptions import InvalidTransaction
from sawtooth_sdk.processor.exceptions import InternalError
import Devicestate_pb2
import address_calculator
import trust_query_pb2

# Initialize logger
LOGGER = logging.getLogger(__name__)



def handleTrustQuery(context, payload, sender):
    LOGGER.info('Trust query received from %s.', sender)

    # Read received query back to TrustQuery object
    trustQuery = trust_query_pb2.TrustQuery()
    trustQuery.ParseFromString(payload)
    requestor = trustQuery.Trustor
    target = trustQuery.Trustee  # Prover#
    # Validate trust query correctness and preceed
    if _validate_trust_query(context, trustQuery, sender):
       trustScore = _Compute_state(context, target)
       ResponseTQ = response_TQ_pb2.responseTQ(
           Result = "valid",
           ProverID=str(target),
           RequestorID=str(requestor),
           TrustStatus=trustScore
       ).SerializeToString()
       context.add_receipt_data(ResponseTQ)
       context.add_event(
           event_type="attestation/trustStatus",
           attributes=[("Result", "valid"),("prover", str(target)), ("requestor", str(requestor)), ("trustStatus", str(trustScore))])
    else:
        ResponseTQ = response_TQ_pb2.responseTQ(
            Result="invalid",
        ).SerializeToString()
        context.add_receipt_data(ResponseTQ)
        context.add_event(
            event_type="attestation/trustStatus",
            attributes=[("Result", "valid"),("prover", str(target)), ("requestor", str(requestor))])


### state:
## [o,1], if attestation is valid and not yet expired
### pending = 2, if no attestation yet, or attested and evidence expired
### malicious = 3, if last in last evidence, measurement is not correct

def _Compute_state(context, ProverID):
    storageAddress = address_calculator._assembleAddress(ProverID)
    state_entries = context.get_state([storageAddress])
    if state_entries == []:
        # if no state is saved for the device, create a new one with request = 1, and return
        LOGGER.info('No previous state for device %s', ProverID)
        devicestate = Devicestate_pb2.DeviceState()
        devicestate.DeviceIdentity = ProverID
        devicestate.Request = 1
        state_data = devicestate.SerializeToString()
        LOGGER.info('Request Flag Set for Device: %s', ProverID)
        addresses = context.set_state({storageAddress: state_data})
        if len(addresses) < 1:
            raise InternalError("State Error")
        # return 2 , confirm request is saved
        return 2
    else:
        devicestate = Devicestate_pb2.DeviceState()
        devicestate.ParseFromString(state_entries[0].data)
        # if device is malicious ( it sent wrong measurement=> strikes >0)
        if devicestate.State == "untrusted":
            LOGGER.info('TrustQuery : device %s is untrusted ', ProverID)
            return 3
        else:
            return _calculate_Reliability(context, ProverID)



def _calculate_Reliability(context , ProverID):
    # retrieve state of device:
    storageAddress = address_calculator._assembleAddress(ProverID)
    state_entries = context.get_state([storageAddress])
    devicestate = Devicestate_pb2.DeviceState()
    devicestate.ParseFromString(state_entries[0].data)
    # retrieve device properties
    Prover = storage_functions.findDeviceProperties(context,ProverID)
    if Prover == []:
        LOGGER.info('Properties List is empty')
    try:
        reliabilityScore = Prover.ReliabilityScore
        timeFunction = Prover.TimeFunction
        xmin = Prover.xmin
        xmax = Prover.xmax
    except AttributeError:
        raise InvalidTransaction('Could not find properties attributes for evidence')
        # Calculate the evidence age
    BlockNum = block_info_functions.getBlockNumber(context, devicestate.LastEvidence)
    if BlockNum == -1 :
        _storeRequest(context, ProverID)
        return 2
    Timestamp = block_info_functions.readBlockTime(context, BlockNum)
    x = _getTimeDifference(context, Timestamp)
    assert xmin <= xmax
    # Calculate the resulting trust score
    if (x <= xmin):
        trustScore = 1
    elif (xmin < x < xmax):
        # Parse and evaluate time function:
        # eval() does not need to be sanitized, administration transaction family input only by administrators
        trustScore = eval(_dedent_string(timeFunction))
    else:
        # if attestation is too old
        _storeRequest(context, ProverID)
        return 2
    # In addition to the time influence, add static reliability influence
    finalTrustScore = trustScore * reliabilityScore
    return finalTrustScore




# check if there s a state saved for the device
# if no state, then create one and save it with req = 1
# if it exists change Request to 1
def _storeRequest(context, ProverID):
    # Address of state of Prover
    storageAddress = address_calculator._assembleAddress(ProverID)
    state_entries = context.get_state([storageAddress])
    if state_entries == []:
        LOGGER.info('No previous state for device %s', ProverID)
        devicestate = Devicestate_pb2.DeviceState()
        devicestate.DeviceIdentity= ProverID
        devicestate.Request = 1
    else :
        devicestate = Devicestate_pb2.DeviceState()
        devicestate.ParseFromString(state_entries[0].data)
        devicestate.Request = 1

    state_data = devicestate.SerializeToString()
    LOGGER.info('Storing New Request: %s', state_data)
    addresses = context.set_state({storageAddress: state_data})
    # Check if data was actually written to addresses
    if len(addresses) < 1:
        raise InternalError("State Error")



# Helper function to format a timestamp in a readable way
def formatTimestamp(timestamp):
    return datetime.datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S')


# Dedents a given string
# Needed for the attestation properties function parser (eval) in calculateEdgeTrustScore
def _dedent_string(string):
    if string and string[0] == '\n':
        string = string[1:]
    return textwrap.dedent(string)


# Calculates the temporal proximity of evidence submission time and current time
def _getTimeDifference(context, timestamp):
    currentTimestamp = block_info_functions.readLastBlockTime(context)
    timeDiff = currentTimestamp - timestamp
    return timeDiff


# Validation of trust query transaction
def _validate_trust_query(context, trustQuery, sender):

    # 1. IDvrf and IDprv are both legitimate participating peers

    try:
        assert (_validate_Dev(context, trustQuery.Trustor) == True)
    except:
        raise InvalidTransaction('Requestor Assertion Error')
    try:
        assert (_validate_Dev(context, trustQuery.Trustee) == True)
    except:
        raise InvalidTransaction('Requestee Assertion Error')
    return True



# Verify if device is on the database
def _validate_Dev(context, DevID):
    deviceList = storage_functions.fetchDeviceList(context)
    if deviceList == []:
        LOGGER.info('Device List is empty')
    else:
        for device in deviceList.Device:
            if (DevID == device.DeviceIdentity):
                return True
    return False
