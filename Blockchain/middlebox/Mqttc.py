#!/usr/bin/python
# -*- coding: utf-8 -*-


import csv
import requests
import yaml
import time
import json
import paho.mqtt.client as mqtt
import logging
from colorlog import ColoredFormatter

DEFAULT_URL = 'http://rest-api:8008'
BROKER = "broker"
LOGGER = logging.getLogger(__name__)


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


def _send_to_rest_api(suffix, data=None, content_type=None):
    '''Send a REST command to the Validator via the REST API.
       Called by _wrap_and_send().
    '''
    url = "{}/{}".format(DEFAULT_URL, suffix)
    print("URL to send to REST API is {}".format(url))

    headers = {}

    if content_type is not None:
        headers['Content-Type'] = content_type

    try:
        if data is not None:
            result = requests.post(url, headers=headers, data=data)
        else:
            result = requests.get(url, headers=headers)

        if not result.ok:
            raise Exception("Error {}: {}".format(
                result.status_code, result.reason))
    except requests.ConnectionError as err:
        raise Exception(
            'Failed to connect to {}: {}'.format(url, str(err)))
    except BaseException as err:
        raise Exception(err)

    return result.text


def _wait_for_status(batch_id, wait, result):
    '''Wait until transaction status is not PENDING (COMMITTED or error).

       'wait' is time to wait for status, in seconds.
    '''
    if wait and wait > 0:
        waited = 0
        status = 'PENDING'
        start_time = time.time()
        while waited < wait:
            result = _send_to_rest_api("batch_statuses?id={}&wait={}"
                                       .format(batch_id, wait))
            print(result)
            #status = yaml.safe_load(result)['data'][0]['status']
            status = 'found'
            waited = time.time() - start_time

            if status != 'PENDING':
                return result
        return "Transaction timed out after waiting {} seconds." \
            .format(wait)
    else:
        return result


def get_receipt(TxID,wait,result):

    result = 'PENDING'
    if wait and wait > 0:
        waited = 0
        start_time = time.time()
        while waited < wait:
            result = _send_to_rest_api("receipts?id={}"
                                       .format(TxID))
            print(result)
            status = result
            waited = time.time() - start_time

            if result != 'PENDING':
                return result
        return "Transaction timed out after waiting {} seconds." \
            .format(wait)
    else:
        return result


class middlebox(object):
    '''
    Client Attestation Manager class handles the the submission of transactions
    Supports "submitEvidence" and "trustQuery" functions.
    '''

    def __init__(self):
        self.mydict=self.loaddict('mapping.csv')
        verbose_level = 0
        setup_loggers(verbose_level=verbose_level)
        self.mqttc = mqtt.Client()
        # Add message callbacks that will only trigger on a specific subscription match.
        self.mqttc.message_callback_add("trustmngt/Batch", self.on_message_Batch)
        self.mqttc.message_callback_add("trustmngt/admin", self.on_message_admin)
        self.mqttc.on_message = self.on_message
        self.mqttc.connect(BROKER, 1883, 60)
        self.mqttc.subscribe("trustmngt/#", 0)
        self.mqttc.loop_forever()
        # read conf file with list of topics to communicate with devices

    def loaddict(self,file):
        with open(file, mode='r') as infile:
            reader = csv.reader(infile)
            dict = {rows[0]: rows[1] for rows in reader}
            LOGGER.info('Loading dictionary %s.',
                        dict)
            return dict
        # This callback will only be called for messages with topics that match
        # trustmngt/Batch#
        # All BatchLists coming from devices or administration are then forwarded to REST-API
        # Status is queried after wait and then sent back to sender

        # message received: json {client_id: device1, batch_id:jdkken455, batch_list:4455535445}

    def on_message_Batch(self,mosq, obj, msg):
        # m_decode = str(msg.payload.decode("utf-8", "ignore"))
        LOGGER.info('Batch received')
        LOGGER.info(' %s.',msg)
        
        m_decode = msg.payload.decode("utf-8", "ignore")
        LOGGER.info(' %s.',m_decode)
        m_in = json.loads(m_decode)  # decode json data
        LOGGER.info(' %s.',m_in)
        Batch_list = m_in['batch_list']
        Batch_ID = m_in['batch_id']
        Device_ID = m_in['device_id']
        Transaction_ID= m_in['transaction_id']
        dict = self.mydict
        if Device_ID in self.mydict:
            result = _send_to_rest_api("batches",
                                       Batch_list,
                                       'application/octet-stream')
            wait = 3
            output = _wait_for_status(Batch_ID, wait, result)

            print("result is:")
            print(output)



            receipt = get_receipt(Transaction_ID,wait,result)
            print("receipt is:")
            print(result)
            data = {}
            data['Answer'] = receipt
            data['Transaction_id'] = Transaction_ID
            json_data = json.dumps(data)
            LOGGER.info('Publishing to %s.',
                        self.mydict.get(Device_ID))
            mosq.publish(self.mydict.get(Device_ID), json_data)

        # add topic to dictionary for new devices

    def on_message_admin(self,mosq, obj, msg):
        LOGGER.info('new device')
        m_decode = msg.payload.decode("utf-8", "ignore")
        m_in = json.loads(m_decode)  # decode json data
        device = str(m_in['device_id'])
        topic = (m_in['topic'])
        self.mydict[device] = topic
        print(self.mydict)

    def on_message(self,mosq, obj, msg):
        # This callback will be called for messages that we receive that do not
        # match any patterns defined in topic specific callbacks,
        print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

def main( args=None):
    '''Entry point function for the client CLI.'''
    verbose_level = 0
    setup_loggers(verbose_level=verbose_level)
    Middlebox = middlebox()

if __name__ == '__main__':
    main()
