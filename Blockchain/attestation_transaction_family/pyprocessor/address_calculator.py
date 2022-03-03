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

import hashlib

FAMILY_NAME = "attestation"

# Hash and assemble address
def _assembleAddress(public_key):

    return _hash(FAMILY_NAME.encode('utf-8'))[0:6] + \
                 _hash(public_key.encode('utf-8'))[0:64]

# Assemble the correct address for a given key using the transaction family name
def _assembleEvidenceStorageAddress(evidence):
    return _assembleAddress(evidence.ProverIdentity)

def _assembleRequestsStorageAddress(evidence):
    return _assembleAddress(evidence.Trustee+"request")

def _assembleTimestampStorageAddress(evidence):
    return _assembleAddress(evidence.ProverIdentity+"timestamp")

def _assembleTimestampQueryAddress(evidence):
    return _assembleAddress(evidence.Trustee+"timestamp")

def _assembleEvidenceQueryAddress(evidence):
    return _assembleAddress(evidence.Trustee)
# Hashing function
def _hash(data):
    '''Compute the SHA-512 hash and return the result as hex characters.'''
    return hashlib.sha512(data).hexdigest()
