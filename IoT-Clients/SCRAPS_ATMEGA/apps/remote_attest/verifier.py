#!/usr/bin/env python3
import sys, os, binascii
import hashlib

sys.path += [os.path.join(os.path.split(__file__)[0], 'libs')]
import serial
from intelhex import IntelHex
from ecdsa.src.ecdsa import VerifyingKey, SECP256k1

# 64 byte key
public_key = 'f837ef7845c5e1f9e8e82df62ff37f5865a17b61b4b055e0759edbd6751018668616f5f6dbad5007d899ff523e5161d0343f0582e9180892da151ca71cc3438e'


def main(argv):
     
    k = 0
    try:
        k = (int(argv[1]))
        if not k > 0:
            raise Exception()
    except:
        print("ERROR: Last argument needs to be a positive integer.")
        sys.exit(2)

    ser = serial.Serial(argv[0], 57600)
    nonces = []
    #print("Nonce(s): ")
    for _ in range(k):
        ni = os.urandom(20)
        while b'.' in ni or b',' in ni or b';' in ni or b'\x0a' in ni or b'\x0d' in ni:
            ni = os.urandom(20)
        #print(binascii.hexlify(ni))
        nonces.append(ni)

    #Write nonces
    for ni in nonces:
        ser.write(ni)
        if ni == nonces[k - 1]:
            ser.write(b'.')  # a '.' at the end
        else:
            ser.write(b',')  # a ',' between each two
    simtype = str(argv[2])
    if  simtype == "checkRequest" :
        ser.write(b',')
        ser.write(b',')
        ser.write(b',')
    elif simtype == "trustQuery":
        ser.write(b':')
        ser.write(b':')
        ser.write(b':')
    else:
        ser.write(b'#')
        ser.write(b'#')
        ser.write(b'#')
    
    answer = ser.read()
    raw_response = []
    while not (answer == b';'):
        answer = ser.read()
        #print(answer)
    print("Answer received.")

if __name__ == "__main__":
    main(sys.argv[1:])
