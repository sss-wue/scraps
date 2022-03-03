#! /usr/bin/env python3

# Copyright 2017-2018 Intel Corporation
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
# ------------------------------------------------------------------------------
#
# Parts of code and comments contained in this file are taken from 
# the official Hyperledger Sawtooth documentation
# https://sawtooth.hyperledger.org/docs/core/releases/1.1.4/contents.html
# and from example projects from developer ``danintel'':
# https://github.com/danintel/sawtooth-cookiejar
#
'''Attestation Family event client

   To run, start the validator then type the following on the command line:
       ./events_client.py

   For more information, see
   https://sawtooth.hyperledger.org/docs/core/releases/latest/app_developers_guide/event_subscriptions.html
'''

import sys
import traceback
import attmgr_client
from sawtooth_sdk.messaging.stream import Stream
from sawtooth_sdk.protobuf import events_pb2
from sawtooth_sdk.protobuf import client_event_pb2
from sawtooth_sdk.protobuf.validator_pb2 import Message

# hard-coded for simplicity (otherwise get the URL from the args in main):
# For localhost access:
#DEFAULT_VALIDATOR_URL = 'tcp://localhost:4004'
# For Docker access:
DEFAULT_VALIDATOR_URL = 'tcp://validator:4004'

# Calculated from the 1st 6 characters of SHA-512("attestation"):
ATTESTATION_TP_ADDRESS_PREFIX = 'fadc96'

# Method to subscribe to the desired events
def subscribe_to_events(delta_filters=None):
    '''Listen to attestation state-delta events.'''


    # Subscribe to events
    trust_path_subscription = events_pb2.EventSubscription(
        event_type="attestation/trustpath", filters=delta_filters)
    trust_entry_subscription = events_pb2.EventSubscription(
        event_type="attestation/entrypoint", filters=delta_filters)
    request = client_event_pb2.ClientEventsSubscribeRequest(
        subscriptions=[trust_path_subscription, trust_entry_subscription])


    # Send the subscription request
    stream = Stream(DEFAULT_VALIDATOR_URL)
    msg = stream.send(message_type=Message.CLIENT_EVENTS_SUBSCRIBE_REQUEST,
                      content=request.SerializeToString()).result()
    assert msg.message_type == Message.CLIENT_EVENTS_SUBSCRIBE_RESPONSE

    # Parse the subscription response
    response = client_event_pb2.ClientEventsSubscribeResponse()
    response.ParseFromString(msg.content)
    assert response.status == \
           client_event_pb2.ClientEventsSubscribeResponse.OK

# Called from the client after submitting a transaction
def listen_to_events():
    # Listen for events in an infinite loop
    stream = Stream(DEFAULT_VALIDATOR_URL)
    print("Listening to events.")
    while True:
        msg = stream.receive().result()
        assert msg.message_type == Message.CLIENT_EVENTS

        # Parse the response
        event_list = events_pb2.EventList()
        event_list.ParseFromString(msg.content)
        print("Received the following events: ----------")
        for event in event_list.events:
            print(event)

# Unsubscription method
def unsubscribe_from_events():
    # Unsubscribe from events
    stream = Stream(DEFAULT_VALIDATOR_URL)
    request = client_event_pb2.ClientEventsUnsubscribeRequest()
    msg = stream.send(Message.CLIENT_EVENTS_UNSUBSCRIBE_REQUEST,
                      request.SerializeToString()).result()
    assert msg.message_type == Message.CLIENT_EVENTS_UNSUBSCRIBE_RESPONSE

    # Parse the unsubscribe response
    response = client_event_pb2.ClientEventsUnsubscribeResponse()
    response.ParseFromString(msg.content)
    assert response.status == \
           client_event_pb2.ClientEventsUnsubscribeResponse.OK
