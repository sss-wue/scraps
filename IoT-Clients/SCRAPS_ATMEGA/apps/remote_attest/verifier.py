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
    if len(argv) != 3:
        print('verifier.py <hexfile> <serialport> <repetitions>')
        sys.exit(2)

    # Check if hexfile exists
    hexfile = argv[0]
    if not os.path.isfile(hexfile):
        print("ERROR: File not found:", hexfile)
        sys.exit(2)
    ih = IntelHex(hexfile)

    # Check if last argument is an integer
    k = 0
    try:
        k = (int(argv[2]))
        if not k > 0:
            raise Exception()
    except:
        print("ERROR: Last argument needs to be a positive integer.")
        sys.exit(2)

    ser = serial.Serial(argv[1], 57600)

    # We do not allow ',' ';' '.' '\x0a' '\x0d' in the nonces, The first three are delimiters, and the rest causes
    # problems as that are ASCII control characters.

    # Generate nonces
    nonces = []
    #print("Nonce(s): ")
    for _ in range(k):
        ni = os.urandom(20)
        while b'.' in ni or b',' in ni or b';' in ni or b'\x0a' in ni or b'\x0d' in ni:
            ni = os.urandom(20)
        #print(binascii.hexlify(ni))
        nonces.append(ni)

    # Write nonces
    for ni in nonces:
        ser.write(ni)
        if ni == nonces[k - 1]:
            ser.write(b'.')  # a '.' at the end
        else:
            ser.write(b',')  # a ',' between each two
    #print("Nonce(s) sent.")

    # Format of answer: "sign(1)sign(2)...sign(64) ;; res(1)res(2)...res(20) ;; ..., n(i 1)n(i 2)...n(i 20), ... .."

    answer = ser.read()
    raw_response = []
    while not (answer == b'.' and raw_response[-1] == b'.'):
        raw_response.append(answer)
        answer = ser.read()
        print(answer)
    print("Answer received.")

'''
    parsed_response = b''.join(raw_response[:-1]).split(b';;')

    if not len(parsed_response) == 3:
        print("ATTESTATION FAILED: Did not receive exactly 3 items.")
        print("Maybe an item started or ended with a '.' or a ';'.")
        sys.exit(2)

    Sign = parsed_response[0]
    res = parsed_response[1]
    N = parsed_response[2].split(b',')

    # Check that all nonces are there
    missing_nonce_generator = (ni for ni in nonces if ni not in N)
    if not len(list(missing_nonce_generator)) == 0:
        print("ATTESTATION FAILED: Nonce(s) missing from the received list.")
        sys.exit(2)

    n = hash_chain(N)

    while len(res) > 20 and b'\x0d\x0a' in res:
        res = res.replace(b'\x0d\x0a', b'\x0a', 1)

    expected_mem = hash_chain([ih.tobinstr(0, 128 * 1024 - 1), n])
    if not res == expected_mem:
        print("ATTESTATION FAILED: Hash of memory and nonce aggregate did not match.")
        print("Expected: ", binascii.hexlify(expected_mem))
        print("Received: ", binascii.hexlify(res))
        sys.exit(2)

    while len(Sign) > 64 and b'\x0d\x0a' in Sign:
        Sign = Sign.replace(b'\x0d\x0a', b'\x0a', 1)

    vk = VerifyingKey.from_string(bytes.fromhex(public_key), curve=SECP256k1)
    if not vk.verify_digest(Sign, res):
        print("ATTESTATION FAILED: Could not verify signature.")
        print("Received: ", binascii.hexlify(Sign))
        sys.exit(2)

    print("ATTESTATION SUCCESSFUL.")


def hash_chain(items):
    h = hashlib.sha1()
    for e in items:
        h.update(e)
    return h.digest()

'''
if __name__ == "__main__":
    main(sys.argv[1:])
