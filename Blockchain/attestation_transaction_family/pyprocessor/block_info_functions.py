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
import block_info_pb2
import time
import datetime

from sawtooth_sdk.processor.handler import TransactionHandler
from sawtooth_sdk.processor.exceptions import InvalidTransaction
from sawtooth_sdk.processor.exceptions import InternalError

# Initialize logger
LOGGER = logging.getLogger(__name__)

# Reads the current timestamp
def readLastBlockTime(context):
    return readBlockTime(context, readLastBlockNumber(context))

# Reads the last block number from BlockInfoConfig
def readLastBlockNumber(context):
    blockInfoAddress = '00b10c01' + 62*'0'
    try:
        state_entries = context.get_state([blockInfoAddress])
        blockInfoConfigEncoded = state_entries[0].data
    except:
        raise InternalError('Failed to load LastBlockNumber')
    blockInfoConfig = block_info_pb2.BlockInfoConfig()
    blockInfoConfig.ParseFromString(blockInfoConfigEncoded)
    lastBlockNumber = blockInfoConfig.latest_block
    return lastBlockNumber



def readLastBlockID(context):
    BlockNumber = readLastBlockNumber(context)
    blockInfoAddress = '00b10c00' + hex(BlockNumber)[2:].zfill(62)
    state_entries = context.get_state([blockInfoAddress])
    blockInfoEncoded = state_entries[0].data
    blockInfo = block_info_pb2.BlockInfo()
    blockInfo.ParseFromString(blockInfoEncoded)
    lastBlockID = blockInfo.previous_block_id
    return lastBlockID


# Reads the time of any block with BlockNumber
def readBlockTime(context, BlockNumber):
    blockInfoAddress = '00b10c00' + hex(BlockNumber)[2:].zfill(62)
    state_entries = context.get_state([blockInfoAddress])
    blockInfoEncoded = state_entries[0].data
    blockInfo = block_info_pb2.BlockInfo()
    blockInfo.ParseFromString(blockInfoEncoded)
    blocktime = blockInfo.timestamp
    st = datetime.datetime.fromtimestamp(blocktime).strftime('%Y-%m-%d %H:%M:%S')
    blocknumber = blockInfo.block_num
    LOGGER.info('BlockNumber: %s, Timestamp: %s , ID: %s', blocknumber, st,blockInfo.previous_block_id)

    return blocktime

# Debugging method to read all known block timestamps
def printAllBlockTimestamps(context):
    latestBlockNumber = readLastBlockNumber(context)
    for i in range(1,latestBlockNumber+1):
        readBlockTime(context, i)
        LOGGER.info('i: %s',
                i)

def getBlockNumber(context,block_id):
    LOGGER.info('searching for block ID  %s', block_id)
    latestBlockNumber = readLastBlockNumber(context)
    i = latestBlockNumber
    while i > 1:
        blockInfoAddress = '00b10c00' + hex(i)[2:].zfill(62)
        state_entries = context.get_state([blockInfoAddress])
        blockInfoEncoded = state_entries[0].data
        blockInfo = block_info_pb2.BlockInfo()
        blockInfo.ParseFromString(blockInfoEncoded)
        LOGGER.info('Checking Block ID %s, against ID  %s', blockInfo.previous_block_id, block_id)
        if blockInfo.previous_block_id == block_id:
            LOGGER.info('getBlockNumber Block Number %s, for ID  %s', i , block_id)

            return i
        i = i-1
    return 0

def getBlockNumber_maxed(context, block_id, xmax):
    latestBlockNumber = readLastBlockNumber(context)
    i = latestBlockNumber
    #time = readBlockTime(context, i)
    currentTimestamp = readLastBlockTime(context)
    Time = 0
    while (i > 1 & (xmax>=Time) ) :
        blockInfoAddress = '00b10c00' + hex(i)[2:].zfill(62)
        state_entries = context.get_state([blockInfoAddress])
        blockInfoEncoded = state_entries[0].data
        blockInfo = block_info_pb2.BlockInfo()
        blockInfo.ParseFromString(blockInfoEncoded)
        if blockInfo.previous_block_id == block_id:
            LOGGER.info('Checking block  %s', blockInfo.previous_block_id)
            LOGGER.info('getBlockNumber Block Number %s, for ID  %s', i, block_id )
            i = i -1
            return i
        i = i-1
        Time = currentTimestamp - readBlockTime(context, i)
    return -1