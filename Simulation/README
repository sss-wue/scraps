This directory includes the code used in the simulations used for performance evaluation.
"graph_search.py" is the implementation of the the graph search used in Legiot.

# Requirements:

```console
$ pip3 install -r requirements.txt 

```
# Simulations
To run the simulation use the following command:

```console
$ python3 simulator.py <systemType> <simType> <simRepetitions> <simDuration> <rate>

```

Parameters:
-	***systemType*** :  	  The system you want to simulate (legiot or scraps)
-	***simType*** : The type of simulation (setup (simulation of warmup phase), run100 (Iterations until 100% hit rate), hitrate, maxquery)
-	***simRepetitions*** : Repetitions of the simulation (usually 6) for run100 the maximum iterations must be defined here (usually 999)
-	***simDuration*** :    Duration each iteration of the simulation in seconds (1 for run 100 for others usually 1200)
-	***rate*** :            Defines the query rate (n/rate e.g rate 2 simulates the system with a query rate of n/2)


## Example:
```console
$ python3 simulator.py  scraps hitrate 1 1 5

```
The output is shown in the following picture.
![Blockchain](simulation.png)
