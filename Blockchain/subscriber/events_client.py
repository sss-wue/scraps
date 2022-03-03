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
'''Subscriber Event Client
   Subscribes to all events in the network.
   Can be used to output the current trust graph and count QueryHits and Misses.

   To run, start the validator then type the following on the command line:
       ./events_client.py

   For more information, see
   https://sawtooth.hyperledger.org/docs/core/releases/latest/app_developers_guide/event_subscriptions.html
'''

import sys
import traceback
import csv
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


def listen_to_events(delta_filters=None):
    '''Listen to all state-delta events from the attestation TF.'''

    trustQueryHits = 0
    trustQueryMisses = 0

    # Subscribe to events
    evidence_submission_subscription = events_pb2.EventSubscription(
        event_type="attestation/evidence_submission", filters=delta_filters)
    evidence_deletion_subscription = events_pb2.EventSubscription(
        event_type="attestation/evidence_deletion", filters=delta_filters)
    trust_path_subscription = events_pb2.EventSubscription(
        event_type="attestation/trustpath", filters=delta_filters)
    trust_entry_subscription = events_pb2.EventSubscription(
        event_type="attestation/entrypoint", filters=delta_filters)
    request = client_event_pb2.ClientEventsSubscribeRequest(
        subscriptions=[evidence_submission_subscription, evidence_deletion_subscription, trust_path_subscription, trust_entry_subscription])


    '''
    block_commit_subscription = events_pb2.EventSubscription(
        event_type="sawtooth/block-commit")
    state_delta_subscription = events_pb2.EventSubscription(
        event_type="sawtooth/state-delta", filters=delta_filters)
    request = client_event_pb2.ClientEventsSubscribeRequest(
        subscriptions=[block_commit_subscription, state_delta_subscription])
    '''


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

    # Listen for events in an infinite loop
    print("Listening to events.")
    lastevent = None
    while True:
        msg = stream.receive().result()
        assert msg.message_type == Message.CLIENT_EVENTS

        # Parse the response
        event_list = events_pb2.EventList()
        event_list.ParseFromString(msg.content)
        print("Received the following events: ----------")
        for event in event_list.events:
            if event == lastevent:
                continue
            else:
                 lastevent = event
            print(event)
            if (event.event_type == "attestation/evidence_submission"):
                vrf = event.attributes[0].value
                prv = event.attributes[1].value
                writeEdgeData(vrf, prv)
            elif (event.event_type == "attestation/evidence_deletion"):
                vrf = event.attributes[0].value
                prv = event.attributes[1].value
                deleteEdgeData(vrf, prv)
            elif (event.event_type == "attestation/trustpath"):
                trustQueryHits +=1
            elif (event.event_type == "attestation/entrypoint"):
                trustQueryMisses +=1
        
    # Unsubscribe from events
    request = client_event_pb2.ClientEventsUnsubscribeRequest()
    msg = stream.send(Message.CLIENT_EVENTS_UNSUBSCRIBE_REQUEST,
                      request.SerializeToString()).result()
    assert msg.message_type == Message.CLIENT_EVENTS_UNSUBSCRIBE_RESPONSE

    # Parse the unsubscribe response
    response = client_event_pb2.ClientEventsUnsubscribeResponse()
    response.ParseFromString(msg.content)
    assert response.status == \
           client_event_pb2.ClientEventsUnsubscribeResponse.OK

def writeEdgeData(vrf, prv):
    alreadyContained = False
    with open('../client_simulation/Graph.csv', 'r', newline='') as csvFile:
        reader = csv.DictReader(csvFile)
        for row in reader:
            if ((row['Verifier'] == vrf) and (row['Prover'] == prv)):
                alreadyContained = True
    row = [vrf[0:4], prv[0:4]]
    with open('../client_simulation/Graph.csv', 'a') as csvFile:
        writer = csv.writer(csvFile)
        if alreadyContained == False:
            writer.writerow(row)
    csvFile.close()

def deleteEdgeData(vrf, prv):
    with open('../client_simulation/Graph.csv', 'r') as csvFile:
        data = list(csv.reader(csvFile))
    with open('../client_simulation/Graph.csv', 'w') as csvFile2:
        writer = csv.writer(csvFile2)
        for row in data:
            if ((row[0] != vrf) or (row[1] != prv)):
                writer.writerow(row)

def main():
    '''Entry point function for the client CLI.'''

    filters = [events_pb2.EventFilter(key="address",
                                      match_string=
                                      ATTESTATION_TP_ADDRESS_PREFIX + ".*",
                                      filter_type=events_pb2.
                                      EventFilter.REGEX_ANY)]

    try:
        # To listen to all events, pass delta_filters=None :
        #listen_to_events(delta_filters=filters)
        listen_to_events(delta_filters=None)
    except KeyboardInterrupt:
        pass
    except SystemExit as err:
        raise err
    except BaseException as err:
        traceback.print_exc(file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()
