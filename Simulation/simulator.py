import sys


import networkx as nx

import time
import graph_search
from random import randint


def validate(evidences, query, timeFunction, minReliability):
    if evidences[query] is not None:
        x = time.time() - evidences[query]
        # print(x)
        timeFunc = timeFunction[0]
        xmin = timeFunction[1]
        xmax = timeFunction[2]

        assert xmin <= xmax
        if (x <= xmin):
            return True
        elif (xmin < x <= xmax):
            # parse and evaluate time function: eval() does not need to be sanitized, administration transaction family input only by administrators
            trustScore = eval(graph_search._dedent_string(timeFunc))
            if trustScore < minReliability:
                # delete evidence
                evidences[query] = None
                return False
            else:
                return True
        else:
            evidences[query] = None
            return False
    else:
        return False


if __name__ == '__main__':
    minHit = 70
    args = sys.argv[1:]
    if(len(args) != 5):
        print("Please provide args in following order: <systemType> <simType> <simRepetitions> <simDuration> <rate>")
    systemType = args[0]
    simType = args[1]
    simRepetitions = int(args[2])
    simulationDuration = int(args[3])
    rate = int(args[4])
    G = nx.DiGraph()
    # initialization
    # maximum searchdepth
    securityParameter = 6

    minReliability = 0.80

    # timeFunction contains function, xmin, xmax
    # timeFunction = ['-0.005455*x + 1.05455', 10, 120]
    # timeFunction = ['-0.25*x + 1.25', 1, 5]
    # timeFunction = ['-0.025*x + 1.5', 20, 40]
    # timeFunction = ['-0.025*x + 1.75', 30, 50]
    # timeFunction = ['-0.025*x + 2.0', 40, 60]
    # timeFunction = ['-x + 1.0', 0, 3]
    # timeFunction = ['-0.01*x + 1', 0, 100]
    # timeFunction = ['1', 0, 1]
    # timeFunction = ['-0.02222222*x + 1.022222', 1, 10]

    # 5min -> 1, 10min -> 0.8
    timeFunction = ['-0.0006666667*x + 1.2', 300, 600]
    # 10min -> 1, 20min -> 0.5
    # timeFunction = ['-0.0008333333*x + 1.5', 600, 1200]
    # 10min -> 1, 20min -> 0.80
    # timeFunction = ['-0.0003333333*x + 1.2', 600, 1200]
    # 1min -> 1, 2min -> 0.8
    # timeFunction = ['-0.003333333*x + 1.2', 60, 120]
    # 2min -> 1, 4min -> 0.8
    # timeFunction = ['-0.001666667*x + 1.2', 120, 240]
    # 3min -> 1, 6min -> 0.8
    # timeFunction = ['-0.001111111*x + 1.2', 180, 360]
    # 4min -> 1, 8min -> 0.8
    # timeFunction = ['-0.0008333333*x + 1.2', 240, 480]
    # timeFunction = ['-0.001666667*x + 1.5', 300, 600]
    # timeFunction = ['-0.001666667*x + 2', 600, 1200]
    # timeFunction = ['0.003333333*x + 2', 300, 600]

    nArray = [100, 200, 400, 600, 800, 1000]
    nArray2 = [2000, 3000, 4000, 5000, 7500, 10000, 15000, 20000, 25000]

    evidenceList = {}

    trustQueryHits = 0
    trustQueryMisses = 0
    attestations = 0
    for i in range(0, 6):
        n = nArray[i]
        # maximum number oft trustQueries per second
        trustQueryRate = n / rate
        evidence = [None] * n
        evidenceList = {}
        pending = [None] * n
        G.clear()
        for j in range(0, simRepetitions):

            simulationStart = time.time()
            lastSecond = time.time()
            secondIterations = 0
            while time.time() < (simulationStart + simulationDuration):

                currentSecond = time.time()
                if ((currentSecond - lastSecond) < 1) and (secondIterations < trustQueryRate):
                    secondIterations += 1
                    verifier = randint(0, n - 1)
                    prover = randint(0, n - 1)
                    while verifier == prover:
                        verifier = randint(0, n - 1)
                        prover = randint(0, n - 1)
                    if systemType == "legiot":
                        pathFound, finalRating, entryPoint, path = graph_search.findOptimalPath(prover, verifier,
                                                                                                securityParameter,
                                                                                                minReliability,
                                                                                                evidenceList,
                                                                                                timeFunction)
                    else:
                        pathFound = validate(evidence, prover, timeFunction, minReliability)
                        entryPoint = None
                    if pathFound:
                        trustQueryHits += 1
                    else:
                        trustQueryMisses += 1
                        newEntry = [verifier, entryPoint, time.time()]
                        testEntry = [prover,time.time()]
                        if systemType == "legiot":
                            if entryPoint in evidenceList:
                                tempList = evidenceList.get(entryPoint)
                                tempList.append(newEntry)
                                evidenceList[entryPoint] = tempList
                            else:
                                newList = [newEntry]
                                evidenceList[entryPoint] = newList
                        else:
                     
                            evidence[testEntry[0]] = testEntry[1]
                elif (currentSecond - lastSecond) >= 1:
                    lastSecond = currentSecond
                    secondIterations = 0
                else:
                    currentSecond = time.time()
                    sleepTime = (1 - (currentSecond - lastSecond))
                    time.sleep(sleepTime)
            if simType == "run100":
                if trustQueryMisses == 0:
                    print("Report -----------------------------------------------------------------")
                    print("Simulation type", systemType)
                    print("Iterations: ", j)
                    print("Number of Nodes:", n)
                    print("Report -----------------------------------------------------------------")
                    G.clear()
                    break
                elif j == simRepetitions - 1:
                    print("Report -----------------------------------------------------------------")
                    print("NO HITS")
                    print("Simulation type", systemType)
                    print("Iterations: ", j)
                    print("Number of Nodes:", n)
                    print("Report -----------------------------------------------------------------")
                    break
                print(trustQueryHits+trustQueryMisses)
                print("Iteration is finished !!! -----------------------------------------")
            elif simType == "setup":
                if trustQueryHits / (trustQueryHits + trustQueryMisses) * 100 >= minHit:
                    print("Report -----------------------------------------------------------------")
                    print("Simulation type", systemType)
                    print("Number of Nodes:", n)
                    print("Reached a minimum hitrate of %s in %s iterations: " % (minHit, j))
                    print("Size of graph", len(G.edges))
                    print("Report -----------------------------------------------------------------")
                    G.clear()
                    break
            else:
                print("Report -----------------------------------------------------------------")
                print("Simulation type", systemType)
                print("Number of Nodes:", n)
                print("Security Parameter:", securityParameter)
                print("Minimal Reliability:", minReliability)
                print("Time Function:", timeFunction)
                print("Simulation Duration:", simulationDuration)
                print("Number of TrustQueries:", (trustQueryHits + trustQueryMisses))
                print("Actual Trust Query Rate:", (trustQueryHits + trustQueryMisses) / (time.time() - simulationStart))
                print("Hits:", trustQueryHits)
                print("Misses:", trustQueryMisses)
                print("HitPercentage:", (trustQueryHits / (trustQueryHits + trustQueryMisses)) * 100)
                print("Simulation Duration:", (time.time() - simulationStart))
                print("Report -----------------------------------------------------------------")
            trustQueryHits = 0
            trustQueryMisses = 0
