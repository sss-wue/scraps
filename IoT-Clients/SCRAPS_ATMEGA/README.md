### SμV-Based Attestation


#### Hardware 
- ATMega-1284P Xplained
- Atmel ICE

<img src="pictures/hardware.JPG" width="250" height="250">

#### Software Requirements

- avrdude 
- avr toolchain

#### Setup

- change directory to IoT-Clients/SCRAPS-ATMEGA/apps/remote_attest
    - Adjust the parameters in the Makefile (e.g. MCU, Frequency, flash, etc.)
    - Run: make microvisor.hex from command line
    - Run: make fuse4 from command line
    - Run: make flash from command line
