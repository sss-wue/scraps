import verifier
import sys
import time

def main(argv):
    iterations = int(argv[0])
    simtype    = argv[1]
    for k in range(1, iterations):
        start = time.time()
        verifier.main(['/dev/ttyACM0', 1, simtype])
        print(str(k) + ":" + str(time.time() - start))

if __name__ == "__main__":
    main(sys.argv[1:])
