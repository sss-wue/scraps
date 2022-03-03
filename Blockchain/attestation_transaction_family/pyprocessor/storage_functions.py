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
import evidence_pb2
import device_pb2
import systemconfig_pb2
import address_calculator

from sawtooth_sdk.processor.exceptions import InternalError

# Initialize logger
LOGGER = logging.getLogger(__name__)

# Addresses for global settings to check evidence validity
system_config_address = '5a7526f43437fca1d5f3d0381073ed3eec9ae42bf86988559e98009795a969919cbeca'
devices_address = '5a75264f03016f8dfef256580a4c6fdeeb5aa0ca8b4068e816a677e908c95b3bdd2150'




# Function to load the global system config
def fetchSystemConfig(context):
    state_entries = context.get_state([system_config_address])
    SystemConfig = systemconfig_pb2.Systemconfig()
    try:
        StoredSystemConfig = state_entries[0].data
        SystemConfig.ParseFromString(StoredSystemConfig)
    except:
        raise InternalError('Failed to load system config')
    return SystemConfig


def findDeviceProperties(context,device):
    deviceList = fetchDeviceList(context)
    for Device in deviceList.Device:
        if device == Device.DeviceIdentity:
            return Device
    if deviceList == []:
       LOGGER.info('Devices List is empty')
       return 0
    else:
        for Device in deviceList.Device:
            if device == Device.DeviceIdentity:
                     return Device
    return 0

# Function to load the global device list
def fetchDeviceList(context):
    state_entries = context.get_state([devices_address])
    deviceList = device_pb2.DevicesList()
    try:
        StoredDeviceList = state_entries[0].data
        deviceList.ParseFromString(StoredDeviceList)
    except:
        raise InternalError('Failed to load device list')
    return deviceList


# Loads the right entry for evidence properties
# def findEvidenceProperties(context, evidence):
#     propertiesList = fetchPropertiesList(context)
#     if propertiesList == []:
#         LOGGER.info('Properties List is empty')
#     else:
#         for properties in propertiesList.Properties:
#             if evidence.Trustee == properties.AttestationType:
#                 return properties
#     return


# Load the global Security Parameter
def loadSecurityParameter(context):
    SystemConfig = fetchSystemConfig(context)
    if SystemConfig == []:
        LOGGER.info('System Config is empty')
    else:
        SecurityParameter = SystemConfig.SecurityParameter
    return SecurityParameter

# Delete an evidence from the global state
def _deleteEvidence(context, evidence):
    address = address_calculator._assembleEvidenceStorageAddress(evidence)
    state_entries = context.get_state([address])
    evidenceList = evidence_pb2.EvidenceList()

    newEvidenceList = evidence_pb2.EvidenceList()

    if state_entries != []: 
        try:
            StoredEvidenceList = state_entries[0].data
            evidenceList.ParseFromString(StoredEvidenceList)
        except:
            raise InternalError('Failed to load state data - deleteEvidence')
        
        for currentEvidence in evidenceList.Evidences:
                if (currentEvidence != evidence):
                    newEvidenceList.Evidences.extend([currentEvidence])
        
    state_data = newEvidenceList.SerializeToString()
    addresses = context.set_state({address: state_data})

    # check if data was actually written to addresses
    if len(addresses) < 1:
        raise InternalError("State Error")
    # Add event submission
    context.add_event(
            event_type="attestation/evidence_deletion",
            attributes=[("verifier", str(evidence.VerifierIdentity)), ("prover", str(evidence.ProverIdentity))])


