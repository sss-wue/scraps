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
import address_calculator
import evidence_pb2
import storage_functions
import Devicestate_pb2

import response_SE_pb2
from sawtooth_sdk.processor.exceptions import InvalidTransaction
from sawtooth_sdk.processor.exceptions import InternalError

# Initialize logger
LOGGER = logging.getLogger(__name__)

'''
Handling of attestation evidence submission
Input: 
    context - current blockchain state
    encodedEvidence - submitted evidence from the transaction payload
Output:
    evidence_submission - event that notifies about a successful evidence submission
'''


def handleEvidenceSubmission(context, encodedEvidence):

    # Read received evidence back to Evidence object
    evidence = evidence_pb2.Evidence()
    evidence.ParseFromString(encodedEvidence)
    LOGGER.info('Received Evidence from Prover %s.',
                evidence.ProverIdentity)
    ## discard if prover ID is wrong
    try:
        assert (_validate_prvID(context, evidence.ProverIdentity) == True)
    except:
        raise InvalidTransaction('Prover Assertion Error')
    ## calculate address for device state
    storageAddress = address_calculator._assembleAddress(evidence.ProverIdentity)

    ### cases:
    ### measurement matches, block id exists and newer than (Xmax,from last attestation)
    ### measurement matches, block ID older or does not exist
    ### measurement doesn t match, block ID exists and newer than (Xmax,from last attestation)
    ### measurement doesn t match, block ID older or does not exist

    meas_match = _validate_measurement(context, evidence)
    block_match = _validate_BlockID(context, evidence)
    if (meas_match and block_match):
            Result = "attested"
            _storeEvidence(context, evidence.ProverIdentity, Result, evidence.BlockID)
            context.add_event(
                event_type="attestation/evidence_submission",
                attributes=[("prover", str(evidence.ProverIdentity)), ("result", str(Result)),
                            ("timestamp", str(evidence.BlockID))])
            Response_SE = response_SE_pb2.response_SE(
                Result = "committed",
                State = "attested"
            ).SerializeToString()
            devicestate = Devicestate_pb2.DeviceState()
            devicestate.DeviceIdentity = evidence.ProverIdentity
            devicestate.State = "attested"
            devicestate.Request = 0
            state_data = devicestate.SerializeToString()
            LOGGER.info('Deleting Flag Set for Device: %s', evidence.ProverIdentity)
            addresses = context.set_state({storageAddress: state_data})
            Result = "attested"
            _storeEvidence(context, evidence.ProverIdentity, Result, evidence.BlockID)
            if len(addresses) < 1:
                raise InternalError("State Error")
            context.add_receipt_data(Response_SE)
    else:
        if ((not meas_match) and block_match):
            Result = "valid"
            _storeEvidence(context, evidence.ProverIdentity, Result, evidence.BlockID)
            context.add_event(
                event_type="attestation/evidence_submission",
                attributes=[("prover", str(evidence.ProverIdentity)), ("result", str(Result)),
                            ("timestamp", str(evidence.BlockID))])
            Response_SE = response_SE_pb2.response_SE(
                Result="valid",
                State="untrusted"
            ).SerializeToString()
            devicestate = Devicestate_pb2.DeviceState()
            devicestate.DeviceIdentity = evidence.ProverIdentity
            devicestate.Request = 1
            devicestate.State = "untrusted"
            state_data = devicestate.SerializeToString()
            LOGGER.info('Deleting Flag Set for Device: %s', evidence.ProverIdentity)
            addresses = context.set_state({storageAddress: state_data})
            if len(addresses) < 1:
                raise InternalError("State Error")

            context.add_receipt_data(Response_SE)

        else :
            context.add_event(
                event_type="attestation/evidence_submission",
                attributes=[("prover", str(evidence.ProverIdentity)), ("result", "Not stored, Block ID does not exist or too old"), ("timestamp", str(evidence.BlockID))])
            Response_SE = response_SE_pb2.response_SE(
                Result="invalid",
            ).SerializeToString()

            context.add_receipt_data(Response_SE)



# Function for prover validation
def _validate_prvID(context, prvID):

    DeviceList = storage_functions.fetchDeviceList(context)
    LOGGER.info('getting devices list ')
    if DeviceList == []:
        LOGGER.info('Device List is empty')
        return False
    else:
        for device in DeviceList.Device:
            if (prvID == device.DeviceIdentity):
                return True
    return False

    # Function for prover validation

# Function to verify, if provided measurement matches the one from the device
def _validate_measurement(context, evidence):
    # Retrieve all policy entries at the given address
    devicesList = storage_functions.fetchDeviceList(context)
    if devicesList == []:
        LOGGER.info('Devices List is empty')
    else:
        for Device in devicesList.Device:
            if (evidence.ProverIdentity == Device.DeviceIdentity and evidence.Measurement == Device.Measurement):
                     LOGGER.info('Found a Matching Measurement :)')
                     return True

    LOGGER.info('No Match Found For Measurement: %s', evidence.Measurement)
    return False



def _validate_BlockID(context, evidence):

    ### retrieve Xmax
    Prover = storage_functions.findDeviceProperties(context, evidence.ProverIdentity)
    if Prover == []:
        LOGGER.info('Properties List is empty')
    try:
        xmax = Prover.xmax
    except AttributeError:
        raise InvalidTransaction('Could not find properties attributes for evidence')
    ### check if provided Block ID exists in the range of (current time - Xmax)
    ### No: return false
    ### Yes : check if it newer than the ID saved in last attestation
    numEvidenceBlock = block_info_functions.getBlockNumber_maxed(context, evidence.BlockID, xmax)

    if numEvidenceBlock == -1:
        LOGGER.info('Block ID submitted in evidence is too old or does not exist')
        return False
    else:
    # retrieve block ID of last attestation
    # verify if the provided block ID exists in the margin of xmax, and newer as the last attestation
        storageAddress = address_calculator._assembleAddress(evidence.ProverIdentity)
        # Retrieve all entries at the given address
        state_entries = context.get_state([storageAddress])
        LOGGER.info('Retrieving device state  %s', state_entries)
        devicestate = Devicestate_pb2.DeviceState()

        if state_entries == []:
            LOGGER.info('No Previous Block ID Stored For Device %s',evidence.ProverIdentity)
            LOGGER.info('Check if Block ID is valid %s', evidence.BlockID)
            return True
        else:
            try:
                # save block ID as tempstamp and zero requests
                devicestate.ParseFromString(state_entries[0].data)

                LOGGER.info('Parsing State of   %s', devicestate.DeviceIdentity)
                LOGGER.info('Block ID in Last Attestation %s', devicestate.LastEvidence)
                #block_info_functions.printAllBlockTimestamps(context)
                stroredBlockNumber = block_info_functions.getBlockNumber(context,devicestate.LastEvidence)
                LOGGER.info('Number of stored block is  %s , Num of Block in submitted evidence is %s ', stroredBlockNumber, numEvidenceBlock )

                old = block_info_functions.readBlockTime(context, stroredBlockNumber)
                LOGGER.info('timestamp for old evidence  %s', old)
                new = block_info_functions.readBlockTime(context, numEvidenceBlock)
                LOGGER.info('timestamp for new evidence  %s', new)
                timeDiff = new - old
                LOGGER.info('Diff is   %s',timeDiff)
                if (timeDiff>=0):
                    return True
            except:
                raise InternalError('Failed to Load State Data')

        return False


# change state to untrusted or attested, and store block ID:
## if attested ==> set request to 0
## else ==> set request to 1


def _storeEvidence(context, prover_id, new_state, block_id):
    LOGGER.info('Changing State for Device : %s to %s', prover_id, new_state)
    storageAddress = address_calculator._assembleAddress(prover_id)
    devicestate = Devicestate_pb2.DeviceState()
    devicestate.DeviceIdentity = prover_id
    devicestate.State = new_state
    devicestate.LastEvidence = block_id

    if new_state == "attested":
        devicestate.Request = 0
    else:
        devicestate.Request = 1

    state_data = devicestate.SerializeToString()
    addresses = context.set_state({storageAddress: state_data})

    # Check if data was actually written to addresses
    if len(addresses) < 1:
        raise InternalError("State Error")
