import sys
import textwrap

import matplotlib.pyplot as plt
import networkx as nx
import csv
import time
import graph_search

from random import randint


def _dedent_string(string):
    if string and string[0] == '\n':
        string = string[1:]
    return textwrap.dedent(string)


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
            trustScore = eval(_dedent_string(timeFunc))
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

    G = nx.DiGraph()

    # initialization
    # number of nodes:
    n = 200
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
    nArray2 = [2000, 5000, 10000, 20000, 50000, 100000]
    iterationsPerSetting = 6
    # dictionary:
    # evidence = [0.0] * n

    # dictionary: prover -> (verifier, prover, timestamp)
    # evidenceList = {}
    args = sys.a
    # simulation duration in seconds
    simulationDuration = 5
    simulationSteps = 1000
    # maximum number oft trustQueries per second
    trustQueryRate = n / 2
    # evidence = [None] * n
    trustQueryHits = 0
    trustQueryMisses = 0
    output = {}
    it = 1
    for i in range(0, 6):
        n = nArray[i]
        trustQueryRate = n / 2
        pendingRequests = [None] * n
        evidence = [None] * n
        evidenceList = {}
        for j in range(0, iterationsPerSetting):
            simulationStart = time.time()
            lastSecond = time.time()
            secondIterations = 0
            while (time.time() < (simulationStart + simulationDuration)):
                # for i in range(0,simulationSteps):

                currentSecond = time.time()
                if (((currentSecond - lastSecond) < 1) and (secondIterations < trustQueryRate)):
                    secondIterations += 1
                    trustQuery = randint(0, n - 1)
                    attestationEvidence = randint(0, n - 1)
                    while (trustQuery == attestationEvidence):
                        trustQuery = randint(0, n - 1)
                    pathFound, finalRating, entryPoint, path = graph_search.findOptimalPath(attestationEvidence,
                                                                                            trustQuery,
                                                                                            securityParameter,
                                                                                            minReliability,
                                                                                            evidenceList,
                                                                                            timeFunction)
                    # result = validate(evidence, trustQuery, timeFunction, minReliability)

                    if (pathFound):
                        # print("A path was found! Verifier: " + str(verifier) + " Prover: " + str(prover) + " Path: " + str(path) + " Reliability: " + str(finalRating))
                        trustQueryHits += 1
                    else:
                        # print("No path found! Verifier: " + str(verifier) + " Prover: " + str(prover) + " Entrypoint: " + str(entryPoint))
                        trustQueryMisses += 1
                        pendingRequests[trustQuery] = time.time()

                        newEntry = [trustQuery, entryPoint, time.time()]

                        if ((entryPoint in evidenceList) == True):
                            tempList = evidenceList.get(entryPoint)
                            tempList.append(newEntry)
                            evidenceList[entryPoint] = tempList
                        else:
                            newList = [newEntry]
                            evidenceList[entryPoint] = newList

                        # time.sleep((1/trustQueryRate))
                 elif ((currentSecond - lastSecond) >= 1):
                    lastSecond = currentSecond
                    secondIterations = 0
                    else:  # (secondIterations <= trustQueryRate):
        # sleep, trustQueryRate reached
        currentSecond = time.time()
        sleepTime = (1 - (currentSecond - lastSecond))
        time.sleep(sleepTime)
        # print ("Second Iterations: "+ str(secondIterations))
        # print("sleeping: " + str(sleepTime))

for i in range(0, len(pendingRequests)):
    if trustQueryMisses == 0:
        print("NO Misses")
        sys.exit()
    evidence[i] = pendingRequests[i]
evidenceListLength = 0
for key, value in evidenceList.items():
    for evidence in value:
        G.add_edge(evidence[0], evidence[1])
        evidenceListLength += 1

print("Report -----------------------------------------------------------------")
print("##### System Parameters #####")
print("Number of Nodes:", n)
print("Security Parameter:", securityParameter)
print("Minimal Reliability:", minReliability)
print("Time Function:", timeFunction)
print("Simulation Duration:", simulationDuration)
print("Simulation Steps:", simulationSteps)
print("################################")
print("Number of TrustQueries:", (trustQueryHits + trustQueryMisses))
print("Actual Trust Query Rate:", (trustQueryHits + trustQueryMisses) / (time.time() - simulationStart))
print("Hits:", trustQueryHits)
print("Misses:", trustQueryMisses)
print("HitPercentage:", (trustQueryHits / (trustQueryHits + trustQueryMisses)) * 100)

print("Simulation Duration:", (time.time() - simulationStart))


iteration = "Iteration " + str(it) + "Hitrate"
nodes = "Nodes " + str(it)
queryRate = "Actual Query Rate " + str(it)
output[iteration] = (trustQueryHits / (trustQueryHits + trustQueryMisses)) * 100
output[nodes] = n
output[queryRate] = (trustQueryHits / (trustQueryHits + trustQueryMisses)) * 100
trustQueryHits = 0
trustQueryMisses = 0
print(output)
it += 1
