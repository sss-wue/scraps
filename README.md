#  SCRAPS: Scalable Collective Remote Attestation for Pub-Sub IoT Networks with Untrusted Proxy Verifier

## Blockchain
Implementation of Sawtooth blockchain, including smart contracts and 3 entities implemented also in containers, namely the administrator and 2 clients with roles of Verifer and Prover. The code is deployed using docker and docker-compose. 
The directory contains the code needed to build the test network.
It includes the necessary files to start Sawtooth hyperledger.
Moreover, the implementation for Manufacture client, smart contracts, SCRAPS attestation client and smart contract.


### I. Requirements
- docker
- docker-compose

### II. How to use
#### 1. Build Network

The network is composed of Sawtooth hyperledger, broker ,and different containers running the code for Manufacture client, and IoT devices.
Administrator container hosts the code necessary to upload configuration information to the blockchain. It, furthermore runs the code for Manufacture client.
mes container is used as a prover, while erp is used as a verifier.
Manufacture and Blockchain communicate directly using http. Communication between IoT devices and blockchain is mediated by a broker.

The network is built using the following command


```console
foo@bar:~$ docker-compose -f test.yml up

```
The output is shown in the following picture. The console keeps printing log information submitted by different containers in the network

![Blockchain](Blockchain/pictures/bc.png)

## 2. System configuration:
The next step is to upload the configuration information to Sawtooth. This task is executed from the administrator container.
The following commands are used to login to admin client container (Manufacturer client), upload systemconfig and devices DB.

```console
foo@bar:~$ docker exec -it administrator-client bash
foo@bar:~$ python3 administration.py  loadSystemConfig &&  python3 administration.py  loadDeviceDB 

```
The output shows how the transactions are submitted and the results of the execution of those transactions.
![Manufacturer](Blockchain/pictures/admin.png)

### b. Start MQTT client on rest-api docker

As Sawtooth does not support MQTT, the next step is to run a MQTT middlebox on rest-api container.
The task of the middlebox is to enable MQTT communication between IoT devices and the blockchain.
The following commands are used to login to rest-api container and start the middleware.


```console
foo@bar:~$ docker exec -it sawtooth-rest-api-default bash
foo@bar:~$ cd project/attestation_management/middlebox
foo@bar:~$ python3 Mqttc.py
```

![Middlebox](Blockchain/pictures/mqtt.png)

### 2. Attestation
After the configuration of the network is finished, IoT devices (mes and erp containers) can start using the attestation service provided by the blockchain.
In the following, mes container is used as a prover, while erp as verifier.
#### mes: Check for pending requests 
Next commands are used tologin to mes (prover with ID 073B) container, and check if it has any pending request


 ```console
foo@bar:~$ docker exec -it mes bash
foo@bar:~$ python3 attmgr.py  checkRequest  073B
```
As no other device queried the state of mes (073B), "AttestationRequired" has the value "False".
![Check-Request](Blockchain/pictures/checkreq1.png)
##  erp (verifier with ID 098D) queries state of mes 
In this step, erp, acting as a prover, queries the blockchain for the state of mes.

```console
$ docker exec -it erp bash
foo@bar:~$ python3 attmgr.py  trustQuery   098D  073B   
```
 The output shows the result of the query sent back from the blockchain. "TrustStatus" has the value 2
 , which means attestation is pending. The attestation smart contract sets a flag for mes signaling that its attestation is required.
![Middlebox](Blockchain/pictures/query1.png)

#### mes: check if any pending request,
mes checks if any pending requests on the blockchain once again
```console
foo@bar:~$ python3 attmgr.py  checkRequest  073B
```
In the response, "AttestationRequired" has the value true. "timestamp" field includes the ID of last written block in the blockchain.
![Check-Request2](Blockchain/pictures/checkreq1.png)
#### submit an evidence 
mes uses the ID of the block received in the last check request operation, and submits a new evidence.
```console
foo@bar:~$ python3 attmgr.py  submitEvidence 073B 7A09AB47D4 688e115dcbe148cfadb12af421a028fab25e94586a6906fcfb4818172bedd49d7d07bb33722d179901534e2824ef64e43cd88b7fb4c11f93cf86258b05d0fb47
```
![Evidence](Blockchain/pictures/submit-evedence.png)
#### erp query blockchain for mes state 
```console
foo@bar:~$ python3 attmgr.py  trustQuery   098D  073B   
```
![Query2](Blockchain/pictures/query2.png)
#### Other cases:

- If mes submits evidence with a wrong or old block ID, the result for erp querying the blockchain for its state will have 2 (pending) as a value for "TrustStatus"
- if mes submits evidence with measurement different than the one saved in blockchain (7A09AB47D4), the result for erp querying the blockchain for its state will have 3 (untrusted) as a value for "TrustStatus"


## Simulations

This directory includes the code used in the simulations used for performance evaluation.
"graph_search.py" is the implementation of the the graph search used in Legiot.

### Requirements:

```console
$ pip3 install -r requirements.txt 

```
### Run Simulations
To run the simulation use the following command:

```console
$ python3 simulator.py <systemType> <simType> <simRepetitions> <simDuration> <rate>

```

Parameters:
-   ***systemType*** :        The system you want to simulate (legiot or scraps)
-   ***simType*** : The type of simulation (setup (simulation of warmup phase), run100 (Iterations until 100% hit rate), hitrate, maxquery)
-   ***simRepetitions*** : Repetitions of the simulation (usually 6) for run100 the maximum iterations must be defined here (usually 999)
-   ***simDuration*** :    Duration each iteration of the simulation in seconds (1 for run 100 for others usually 1200)
-   ***rate*** :            Defines the query rate (n/rate e.g rate 2 simulates the system with a query rate of n/2)


#### Example:
```console
$ python3 simulator.py  scraps hitrate 1 1 5

```
The output is shown in the following picture.
![Simulation](Simulation/simulation.png)


## IoT-Clients
C implementation of IoT client in 2 boards (LPCXpresso55S69 + Mikroe WiFi 10 Click and Atmel MEGA-1284P Xplained). 

### TrustZone-Based Attestation

#### Hardware 

- LPCXpresso55S69 
- WIFI 10 Click 
![Hardware](IoT-Clients/LPC55/pictures/hardware.png)

#### Software Requirements

- arm-none-eabi
- MCUXpresso IDE v11.4.1 [Build 6260] [2021-09-15]
- SDK_2.x_LPCXpresso55S69 [API version=2.0.0, Format version=3.8]
![Software](IoT-Clients/LPC55/pictures/software.png)

#### Setup

- Import both application (secure and non secure) as projects in MCUXpresso
- Check that projects are linked as shown in their setting in the following pictures
- Set Wifi credentials on non_secure_application/freertos/demos/include/aws_clientcredential.h
- Set broker topics under non_secure_application/source/remote_control.c 

#### Example
In this example, non_secure_application calls a function from the secure application (running in secure world), to generate a CheckRequest.
The request is built in the secure world as a transaction, signed with the private key( also saved in the secure world). The non_secure_application sends it then to the blockchain using MQTT.


### Microvisor-Based Attestation


#### Hardware 
- ATMega-1284P Xplained
- Atmel ICE
![Hardware](IoT-Clients/SCRAPS_ATMEGA/pictures/hardware.png)

#### Software Requirements

- avrdude 
- avr toolchain

#### Setup

- change directory to IoT-Clients/SCRAPS-ATMEGA/apps/remote_attest
    - Adjust the parameters in the Makefile (e.g. MCU, Frequency, flash, etc.)
    - Run: make microvisor.hex from command line
    - Run: make fuse4 from command line
    - Run: make flash from command line
    - Then, run the python script file verifier.py that serves as a verifier, attaching as parameters the hex file and the serial port of the attached MCU.




