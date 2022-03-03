import time
import textwrap

'''
findOptimalPath function for establishing a path between verifier and prover
BuildPath function in the Sawtooth prototype

Input: 
    proverID - prover key or identity
    verifierID - verifier key or identity
    SecurityParameter - maximum allowed hop distance (search depth)
    minReliability - minimum required reliability for resulting path
    EvidenceList - list of existing evidences
    timeFunction - current simulation time function
Output:
    pathFound - boolean if a final path was found
    finalRating - rating of the path
    entryPoint - node the verifier needs to attest to enter the graph
    path - sequence of nodes that build the final path
'''


def findOptimalPath(proverID, verifierID, SecurityParameter, minReliability, EvidenceList, timeFunction):
    # Initialization of return values
    pathFound = False
    finalRating = 0
    entryPoint = None
    path = None

    # Initialization of search parameters
    maxDepth = SecurityParameter
    currentDepth = 0
    Fringe = []
    newFringe = []
    # dictionary to store visited nodes: reliabilityAtNode, nodeDepth, path
    visited = {}

    # Init for prover
    visited[proverID] = [1, currentDepth, proverID]
    if proverID == verifierID:
        # prover equals verifier
        pathFound = True
        finalRating = 1
        # LOGGER.info('Verifier equals Prover')
        print("Vrf equals Prv")
        return pathFound, finalRating, entryPoint, path

    # Parent layer initiaized, increase currentDepth    
    currentDepth += 1
    Fringe.append(proverID)

    # Expansion until maxDepth
    while currentDepth <= maxDepth:
        # LOGGER.info('Expanding nodes for depth %s', currentDepth)
        # print("currentDepth: " + str(currentDepth))
        for node in Fringe:
            # print("Node:" ,node)
            if not (node in EvidenceList):
                # print("node not in EvidenceList")
                # LOGGER.info('Evidence List is empty')
                continue
            for evidence in EvidenceList[node]:
                # print("EvidenceList[node]:", EvidenceList[node])
                newScore = calculateEdgeTrustScore(EvidenceList, evidence, timeFunction)
                # If evidences with a score of 0 are still contained, they are deleted now. Thus they must not be added to visited[]!
                if newScore == 0:
                    # LOGGER.info('Continuing...')
                    continue
                if currentDepth == 1:
                    parentScore = newScore
                else:
                    parentScore = newScore * float(visited[node][0])

                    # evidence[0] is verifierID, evidence[1] is proverID, evidence[2] is timestamp
                if ((evidence[0] == verifierID) and (float(parentScore) >= minReliability)):
                    # A path to the verifier was found!
                    pathFound = True
                    finalRating = parentScore
                    path = visited[evidence[1]][2]
                    # print('Verifier path was found. TrustScore:' + finalRating + 'with Path: '+ (path + ',' + evidence.VerifierIdentity))
                    return pathFound, finalRating, entryPoint, path
                    # check that you didn't find prover again!
                if (((evidence[0] in visited) == False) and (currentDepth < maxDepth)):
                    visited[evidence[0]] = [float(parentScore), currentDepth,
                                            ((str(visited[evidence[1]][2])) + ',' + str(evidence[0]))]
                    # LOGGER.info('Writing visited key: %s - score: %s - depth: %s', evidence.VerifierIdentity, parentScore, currentDepth)
                    newFringe.append(evidence[0])
        Fringe.clear()
        Fringe.extend(newFringe)
        newFringe.clear()
        currentDepth += 1

    # Debug: Print visited list after completion
    # for node in visited:
    # LOGGER.info('Key: %s, Value: %s', node, visited[node])

    # This part is only reached when no path between verifer and prover was found
    # Calculate the optimal entryPoint here:
    entryPoint, finalRating, path = calculateEntry(visited, minReliability)

    return pathFound, finalRating, entryPoint, path


'''
calculateEntry function to determine the best possible graph entry point

Input: 
    visited - list of visited candidate nodes
    minReliability - minimum required reliability for resulting path
Output:
    entryPoint - node the verifier needs to attest to enter the graph
    reliability - reliability of the final path
    path - the final path
'''


def calculateEntry(visited, minReliability):
    candidates = []
    # print("length_visited: " + str(len(visited)))
    # print(visited)
    # add all nodes that fulfill the minimal reliability requirement to the candidates list
    for key, value in visited.items():
        # LOGGER.info('Node: %s ', node)
        # print(value[0])
        if value[0] > minReliability:
            newCandidate = [key, value[0], value[1], value[2]]
            candidates.append(newCandidate)
    # sort candidates in the following order: 1. Farest distance to prover 2. Highest reliability under equal distances
    candidates.sort(key=_getReliability, reverse=True)
    # now that candidates are sorted by decreasing reliability, sort them by depth:
    candidates.sort(key=_getDepth, reverse=True)
    # LOGGER.info('Candidate found: %s out of all candidates: %s', candidates[0], candidates)
    # print(len(candidates))
    # print("candidates[0]_ID " + str(candidates[0][0]) + " candidates[0]_Rel " + str(candidates[0][1]) + " candidates[0]_Depth " + str(candidates[0][2]) + " candidates[0]_Path " + str(candidates[0][3]))
    return _getNodeID(candidates[0]), _getReliability(candidates[0]), _getPath(candidates[0])


def _getNodeID(elem):
    return elem[0]


def _getReliability(elem):
    return elem[1]


def _getDepth(elem):
    return elem[2]


def _getPath(elem):
    return elem[3]


'''
calculateEdgeTrustScore function to calculate the reliability of a single edge

Input: 
    EvidenceList - list of all evidences
    evidence - the evidence to calculate the score for
    timeFunction - current simulation timefunction
Output:
    trustScore - resulting edge trust score
'''


def calculateEdgeTrustScore(EvidenceList, evidence, timeFunction):
    x = time.time() - evidence[2]
    # print(x)
    timeFunc = timeFunction[0]
    xmin = timeFunction[1]
    xmax = timeFunction[2]

    assert xmin <= xmax
    if (x <= xmin):
        trustScore = 1
    elif (xmin < x <= xmax):
        # parse and evaluate time function: eval() does not need to be sanitized, administration transaction family input only by administrators
        trustScore = eval(_dedent_string(timeFunc))
    else:
        trustScore = 0
        delete_evidence(EvidenceList, evidence)
    return trustScore


def _dedent_string(string):
    if string and string[0] == '\n':
        string = string[1:]
    return textwrap.dedent(string)


def delete_evidence(EvidenceList, evidence):
    newList = []
    tempList = EvidenceList[evidence[1]]
    for iterEvidence in tempList:
        if (iterEvidence != evidence):
            newList.append(iterEvidence)
    EvidenceList[evidence[1]] = newList
