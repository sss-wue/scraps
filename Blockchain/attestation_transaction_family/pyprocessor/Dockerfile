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
# -----------------------------------------------------------------------------

FROM ubuntu:bionic

RUN \
 if [ ! -z $HTTP_PROXY ] && [ -z $http_proxy ]; then \
  http_proxy=$HTTP_PROXY; \
 fi; \
 if [ ! -z $HTTPS_PROXY ] && [ -z $https_proxy ]; then \
  https_proxy=$HTTPS_PROXY; \
 fi; \
 apt-get update \  
 && apt-get install gnupg2 -y \
 && apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 8AA7AF1F1091A5FD \
 && echo "deb [arch=amd64] http://repo.sawtooth.me/ubuntu/chime/stable bionic universe" >> /etc/apt/sources.list \
 && apt-get update \
 && apt-get install -y -q \
    apt-transport-https \
    build-essential \
    ca-certificates \
    python3-sawtooth-sdk \
	python3-cbor \
	python3-pip \
 && apt-get clean \
 && pip3 install -U protobuf==3.6.0 \
 && python3 -m pip install paho-mqtt \
 && rm -rf /var/lib/apt/lists/*

EXPOSE 4004/tcp

WORKDIR /project/attestation_management/attestation_transaction_family/pyprocessor
ENV PATH "$PATH:/project/attestation_management/attestation_transaction_family/pyprocessor"

CMD bash -c 'python3 ./attmgr_tp.py'
