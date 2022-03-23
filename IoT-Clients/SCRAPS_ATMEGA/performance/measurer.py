import verifier
import sys
import time

def main(argv):
    for k in range(83, 100):
        start = time.time()
        verifier.main(['microvisor.hex', '/dev/ttyACM0', 1])
        print(int(k) + ":" + str(time.time() - start))

if __name__ == "__main__":
    main(sys.argv[1:])
