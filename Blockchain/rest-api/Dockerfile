

FROM hyperledger/sawtooth-rest-api:1.0

ARG DEBIAN_FRONTEND=noninteractive

RUN \
 apt update \
 && apt-get install -y python3-requests python3-pip \
 && python3 -m pip install paho-mqtt