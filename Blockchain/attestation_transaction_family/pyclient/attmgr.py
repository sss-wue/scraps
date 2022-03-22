#!/usr/bin/env python3

# Copyright 2018 Intel Corporation
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
'''
Command line interface for attestation TF.
Parses command line arguments and passes to the attmgr_client class
to process.
'''

import argparse
import logging
import os
import sys
import traceback
import evidence_pb2
import trust_query_pb2


from decimal import Decimal
from colorlog import ColoredFormatter
from attmgr_client import AttestationManagerClient
import check_request_pb2

KEY_NAME = 'client1'

# hard-coded for simplicity (otherwise get the URL from the args in main):
broker = "broker"
port = 1883
# Initialize logger
LOGGER = logging.getLogger(__name__)

# Initialize console
def create_console_handler(verbose_level):
    '''Setup console logging.'''
    del verbose_level # unused
    clog = logging.StreamHandler()
    formatter = ColoredFormatter(
        "%(log_color)s[%(asctime)s %(levelname)-8s%(module)s]%(reset)s "
        "%(white)s%(message)s",
        datefmt="%H:%M:%S",
        reset=True,
        log_colors={
            'DEBUG': 'cyan',
            'INFO': 'green',
            'WARNING': 'yellow',
            'ERROR': 'red',
            'CRITICAL': 'red',
        })

    clog.setFormatter(formatter)
    clog.setLevel(logging.DEBUG)
    return clog

# Logger setup
def setup_loggers(verbose_level):
    '''Setup logging.'''
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)
    logger.addHandler(create_console_handler(verbose_level))

# Assembly of parsers for the transaction commands
def create_parser(prog_name):
    '''Create the command line argument parser for the attestation CLI.'''
    parent_parser = argparse.ArgumentParser(prog=prog_name, add_help=False)
    parser = argparse.ArgumentParser(
        description='Provides subcommands to manage attestation transaction family via CLI',
        parents=[parent_parser])

    subparsers = parser.add_subparsers(title='subcommands', dest='command')
    subparsers.required = True

    submitEvidence_subparser = subparsers.add_parser('submitEvidence',
                                           help='submit an attestation evidence',
                                           parents=[parent_parser])
    submitEvidence_subparser.add_argument('prvID',
                                #type=string,
                                help='Prover Public Key')

    submitEvidence_subparser.add_argument('measurement',
                                #type=string,
                                help='Measurement Value')
    submitEvidence_subparser.add_argument('BlockID',
                                #type=string,
                                help='BlockID Value')

                                
    trustQuery_subparser = subparsers.add_parser('trustQuery',
                                           help='Query a trust link',
                                           parents=[parent_parser])
    trustQuery_subparser.add_argument('trustor',
                                #type=string,
                                help='The device to establish trust')	
    trustQuery_subparser.add_argument('trustee',
                                #type=string,
                                help='The device to be attested')

    trustQuery_subparser = subparsers.add_parser('checkRequest',
                                                 help='Check if there is a pending request',
                                                 parents=[parent_parser])
    trustQuery_subparser.add_argument('proverID',
                                      # type=string,
                                      help='The device to establish trust')

    return parser



def CheckRequest(args):
    privkeyfile = _get_private_keyfile(KEY_NAME)
    client = AttestationManagerClient(broker=broker,port=port, device_id=args.proverID, key_file=privkeyfile)
    queryBytes = buildCheckRequestPayload(args.proverID)
    response = client.submitCheckRequest(queryBytes)
    print("Check Request Result: {}".format(response))

# Command to handle an evidence submission from the command line
def submit_evidence(args):
    '''Subcommand to submit an attestation evicende.  Calls client class to do submission.'''
    privkeyfile = _get_private_keyfile(KEY_NAME)
    client = AttestationManagerClient(broker=broker,port=port, device_id=args.prvID, key_file=privkeyfile)
    encodedEvidence = buildEvidencePayload(args.prvID, args.measurement, args.BlockID)
    response = client.submitEvidence(encodedEvidence, args.prvID)
    print("Evidence Submission Result: {}".format(response))



# Command to handle a trust query from the command line
def trustQuery(args):
    privkeyfile = _get_private_keyfile(KEY_NAME)
    client = AttestationManagerClient(broker=broker,port=port, device_id=args.trustor, key_file=privkeyfile)
    queryBytes = buildTrustQueryPayload(args.trustor, args.trustee)
    response = client.submitTrustQuery(queryBytes)
    print("Trust Query Result: {}".format(response))


# Builder method for the evidence object (protobuf)
def buildEvidencePayload(prvID, measurement,blockID):
    encodedEvidence = evidence_pb2.Evidence(
        ProverIdentity = prvID,
        Measurement = measurement,
        BlockID = blockID
        # Timestamp should not be set here because it can only be trusted on the smart contract side
        # If set here it is overwritten from the Transaction Processor
    ).SerializeToString()
    return encodedEvidence

# Builder method for the trust query object (protobuf)
def buildTrustQueryPayload(trustor, trustee):
    trustQuery = trust_query_pb2.TrustQuery(
        Trustor = trustor,
        Trustee = trustee,
        #MinReliability = Decimal(minReliability)
    ).SerializeToString()
    return trustQuery

def buildCheckRequestPayload(deviceID):
    checkRequest = check_request_pb2.Checkrequest(
        DeviceID=deviceID,
    ).SerializeToString()
    return checkRequest


# Load the private keyfile
def _get_private_keyfile(key_name):
    '''Get the private key for key_name.'''
    home = os.path.expanduser("~")
    key_dir = os.path.join(home, ".sawtooth", "keys")
    return '{}/{}.priv'.format(key_dir, key_name)

def main(prog_name=os.path.basename(sys.argv[0]), args=None):
    '''Entry point function for the client CLI.'''
    print("commands examples:")
    print("    submitEvidence 066B 073BAV5O899  f110b....83b0d57")
    print("    trustQuery 0794 073B")
    print("    CheckRequest 0794")

    try:
        #if args is None:
        #   args = sys.argv[1:]

        parser = create_parser(prog_name)
        args = parser.parse_args(args)
        verbose_level = 0
        setup_loggers(verbose_level=verbose_level)
        # Get the commands from cli args and call corresponding handlers
        if args.command == 'submitEvidence':
            submit_evidence(args)
        elif args.command == 'trustQuery':
            trustQuery(args)
        elif args.command == 'checkRequest':
            CheckRequest(args)
        else:
            raise Exception("Invalid command: {}".format(args.command))

    except KeyboardInterrupt:
        pass
    except SystemExit as err:
        raise err
    except BaseException as err:
        traceback.print_exc(file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()
