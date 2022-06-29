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

ARG DEBIAN_FRONTEND=noninteractive

RUN \
 if [ ! -z $HTTP_PROXY ] && [ -z $http_proxy ]; then \
  http_proxy=$HTTP_PROXY; \
 fi; \
 if [ ! -z $HTTPS_PROXY ] && [ -z $https_proxy ]; then \
  https_proxy=$HTTPS_PROXY; \
 fi; \
  apt-get update \  
 && apt-get install gnupg2 -y 

 RUN echo "deb http://repo.sawtooth.me/ubuntu/nightly bionic universe" >> /etc/apt/sources.list \
 && (apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 44FC67F19B2466EA \
 || apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys 44FC67F19B2466EA) \
 && apt-get update \
 && apt-get install -y -q \
    git \
    python3

RUN apt-get install -y -q \
    python3-grpcio \
    python3-grpcio-tools \
    python3-protobuf

RUN apt-get install -y -q \
    python3-colorlog \
    python3-secp256k1 \
    python3-toml \
    python3-yaml \
    python3-zmq

RUN apt-get update \
    && apt-get install -y -q \
    software-properties-common \
    && apt-get install -y -q \
    python3-sawtooth-sdk \
    && apt-get install -y -q \
    python3-sawtooth-cli 


RUN apt-get install -y -q \
    python3-cov-core \
    python3-nose2 \
    python3-pip   \
  && apt-get update \
  && apt-get install -y -q --no-install-recommends \
     apt-utils \
  && apt-get install -y -q \
     apt-transport-https \
     build-essential \
     ca-certificates \
     inetutils-ping \
     libffi-dev \
     libssl-dev \
     python3-aiodns \
     python3-aiohttp \
     python3-aiopg \
     python3-async-timeout \
     python3-bitcoin \
     python3-cbor \
     python3-cchardet \
     python3-chardet \
     python3-cryptography-vectors \
     python3-cryptography \
     python3-dev \
     python3-lmdb \
     python3-multidict \
     python3-netifaces \
     python3-psycopg2 \
     python3-pycares \
     python3-pyformance \
     python3-pytest-runner \
     python3-pytest \
     python3-pytz \
     python3-requests \
     python3-setuptools-scm \
     python3-six \
     python3-yarl \
     python-paho-mqtt \
   && apt-get clean \
   && python3 -m pip install paho-mqtt \
   && pip3 install -U protobuf==3.6.0 \
   && rm -rf /var/lib/apt/lists/*

WORKDIR /project/attestation_management/attestation_transaction_family/pyclient

ENV PATH "$PATH:/project/attestation_management/attestation_transaction_family/pyclient"

EXPOSE 3000

CMD unset PYTHONPATH && python3 setup.py clean --all && python3 setup.py build
