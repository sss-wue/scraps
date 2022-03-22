import logging

from sawtooth_sdk.processor.exceptions import InvalidTransaction
import Devicestate_pb2
import storage_functions
import address_calculator
import block_info_functions
import response_CR_pb2
import check_request_pb2

LOGGER = logging.getLogger(__name__)

def handleCheckRequest(context, payload, sender):
    LOGGER.info('CheckRequest received from %s.',
                sender)

    # Read received request back to CheckRequest object
    checkRequest = check_request_pb2.Checkrequest()
    checkRequest.ParseFromString(payload)
    # Validate check request correctness according to Section 6.4.3
    _validate_check_request(context, checkRequest)
    ProverID = checkRequest.DeviceID
    result = 0
    storageAddress = address_calculator._assembleAddress(ProverID)
    state_entries = context.get_state([storageAddress])
    if state_entries == []:
        LOGGER.info('No previous state for device %s', ProverID)
    else:
        devicestate = Devicestate_pb2.DeviceState()
        devicestate.ParseFromString(state_entries[0].data)
        result = devicestate.Request

    if result == 1:
        ResponseCR = response_CR_pb2.response_CR(
            Timestamp=str(block_info_functions.readLastBlockTime(context)),
            Result = result
        ).SerializeToString()
        context.add_receipt_data(ResponseCR)
        context.add_event(
            event_type="attestation/checkRequest",
            attributes=[("prover", str(checkRequest.DeviceID)), ("timestamp", str(
                block_info_functions.readLastBlockID(context))), ("attestationRequired", str(True))])
    else:
        ResponseCR = response_CR_pb2.response_CR(
            Timestamp=str(0),
            Result = result
        ).SerializeToString()
        context.add_receipt_data(ResponseCR)
        context.add_event(
            event_type="attestation/checkRequest",
            attributes=[("prover", str(checkRequest.DeviceID)), ("timestamp", str(0)) , (("attestationRequired", str(False)))])


# Function for Dev validation
def _validate_Dev(context, DevID):
    LOGGER.info('Handle_request: check for Device : %s',DevID)
    deviceList = storage_functions.fetchDeviceList(context)
    if deviceList == []:
        LOGGER.info('Device List is empty')
    else:
        for device in deviceList.Device:
            if (DevID == device.DeviceIdentity):
                LOGGER.info('Handle_request: Device is found')
                return True
    return False


def _validate_check_request(context, checkRequest):
    try:
        assert (_validate_Dev(context, checkRequest.DeviceID) == True)
    except:
        raise InvalidTransaction('Attestation target Assertion Error')









