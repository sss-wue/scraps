#!/usr/bin/env python3

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
# ------------------------------------------------------------------------------
#
# Parts of code and comments contained in this file are taken from 
# the official Hyperledger Sawtooth documentation
# https://sawtooth.hyperledger.org/docs/core/releases/1.1.4/contents.html
# and from example projects from developer ``danintel'':
# https://github.com/danintel/sawtooth-cookiejar
#
'''
AdministrationTransactionHandler class interfaces for Administration Transaction Family.
'''

import traceback
import sys
import hashlib
import logging
import cbor


from sawtooth_sdk.processor.handler import TransactionHandler
from sawtooth_sdk.processor.exceptions import InvalidTransaction
from sawtooth_sdk.processor.exceptions import InternalError
from sawtooth_sdk.processor.core import TransactionProcessor

import systemconfig_pb2
import device_pb2
import Devicestate_pb2

# hard-coded for simplicity (otherwise get the URL from the args in main):
#DEFAULT_URL = 'tcp://localhost:4004'
# For Docker:
DEFAULT_URL = 'tcp://validator:4004'

LOGGER = logging.getLogger(__name__)

FAMILY_NAME = "administration"
# TF Prefix is first 6 characters of SHA-512("administration"), 5A7526

def _hash(data):
    '''Compute the SHA-512 hash and return the result as hex characters.'''
    return hashlib.sha512(data).hexdigest()


class AdministrationTransactionHandler(TransactionHandler):
    '''
    Transaction Processor class for the Administration Transaction Family.

    This TP communicates with the Validator using the accept/get/set functions.
    This implements functions to update all administration databases.
    '''
    def __init__(self, namespace_prefix):
        '''Initialize the transaction handler class.

           This is setting the "administration" TF namespace prefix.
        '''
        self._namespace_prefix = namespace_prefix
        LOGGER.info("Starting administrator smart contract")

    @property
    def family_name(self):
        '''Return Transaction Family name string.'''
        return FAMILY_NAME

    @property
    def family_versions(self):
        '''Return Transaction Family version string.'''
        return ['1.0']

    @property
    def namespaces(self):
        '''Return Transaction Family namespace 6-character prefix.'''
        return [self._namespace_prefix]

    def apply(self, transaction, context):
        '''This implements the apply function for the TransactionHandler class.

           The apply function does most of the work for this class by
           processing a transaction for the administration transaction family.
        '''

        # Get the payload and extract the administration-specific information.
        # Payload needs to be cbor decoded and split into action and actual (inner) payload
        header = transaction.header        
        action, payload = self._decode_transaction(transaction.payload)

        # Get the signer's public key, sent in the header from the client.
        sender = header.signer_public_key

        # Enable transaction receipts
        b = bytes("adminData", 'utf-8')
        context.add_receipt_data(transaction.payload)

        # Perform the action.
        LOGGER.info("Action = %s.", action)
        LOGGER.info("Payload = %s.", payload)
		
		# Select the appropriate action

        if action == "submitSystemConfig":
            address = handleSystemConfigSubmission(context, payload)
            LOGGER.info("SystemConfig Address = %s", address)
        elif action == "submitDevices":
            address = handleDevicesSubmission(context, payload)
            LOGGER.info("Devices Address = %s", address)
        else:
            LOGGER.info("Unhandled action. Action not legal!")

    # Handle transaction decoding
    def _decode_transaction(self, payload):
        try:
            content = cbor.loads(payload)
        except:
            raise InvalidTransaction('Invalid payload serialization')

        try:
            action = content['Action']
        except AttributeError:
            raise InvalidTransaction('Action must be here')

        try:
            payload = content['Payload']
        except AttributeError:
            raise InvalidTransaction('Payload must be here')

        return action, payload



# Write the system config database
def handleSystemConfigSubmission(context, payload):
    SystemConfig = systemconfig_pb2.Systemconfig()
    SystemConfig.ParseFromString(payload)
    address = _assembleAddress('CONFIG')
    state_data = SystemConfig.SerializeToString()
    LOGGER.info('State Data String: %s',
                        state_data)
    addresses = context.set_state({address: state_data})
    return addresses

# Write the device database
def handleDevicesSubmission(context, payload):
    DeviceList = device_pb2.DevicesList()
    DeviceList.ParseFromString(payload)

    address = _assembleAddress('DEVICES')
    LOGGER.info('Devices are saved at address: %s',
                address)
    state_data = DeviceList.SerializeToString()
    LOGGER.info('State Data String: %s',
                        state_data)
    addresses = context.set_state({address: state_data})
    LOGGER.info('List of Devices is stored')

    # for Device in DeviceList:
    #     Devicestate = Devicestate_pb2.DeviceState()
    #     Devicestate.DeviceIdentity = Device.DeviceIdentity
    #     address = _assembleAddress(Devicestate.DeviceIdentity)
    #     state_data = Devicestate.SerializeToString()
    #     addresses = context.set_state({address: state_data})
    #     LOGGER.info('State saved for : %s',
    #                 Devicestate.DeviceIdentity)
    return addresses


# Assemble storage addresses
def _assembleAddress(storage_target):

    return _hash(FAMILY_NAME.encode('utf-8'))[0:6] + \
             _hash(storage_target.encode('utf-8'))[0:64]

def main():
    '''Entry-point function for the Administration Transaction Processor.'''
    try:
        # Setup logging for this class.
        logging.basicConfig()
        logging.getLogger().setLevel(logging.DEBUG)

        # Register the Transaction Handler and start it.
        processor = TransactionProcessor(url=DEFAULT_URL)
        sw_namespace = _hash(FAMILY_NAME.encode('utf-8'))[0:6]
        handler = AdministrationTransactionHandler(sw_namespace)
        processor.add_handler(handler)
        processor.start()
    except KeyboardInterrupt:
        pass
    except SystemExit as err:
        raise err
    except BaseException as err:
        traceback.print_exc(file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()
