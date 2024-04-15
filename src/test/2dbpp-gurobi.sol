Set parameter TimeLimit to value 30
Set parameter LogFile to value "gurobi.log"
Set parameter Threads to value 1
Using license file /home/linpeng/software/gurobi11/licence/gurobi.lic

Gurobi Optimizer version 11.0.0 build v11.0.0rc2 (linux64 - "Ubuntu 20.04.4 LTS")
Copyright (c) 2023, Gurobi Optimization, LLC

Read MPS format model from file /pub/netdisk1/linpeng/Local-MIP/test/2dbpp.mps
Reading time = 0.04 seconds
: 33603 rows, 22684 columns, 147658 nonzeros

CPU model: AMD EPYC 9654 96-Core Processor, instruction set [SSE2|AVX|AVX2|AVX512]
Thread count: 192 physical cores, 192 logical processors, using up to 1 threads

Optimize a model with 33603 rows, 22684 columns and 147658 nonzeros
Model fingerprint: 0x20e398b2
Variable types: 212 continuous, 22472 integer (22472 binary)
Coefficient statistics:
  Matrix range     [8e-02, 6e+00]
  Objective range  [1e+00, 2e+00]
  Bounds range     [1e+00, 6e+00]
  RHS range        [1e+00, 1e+01]
Found heuristic solution: objective 1.7617570
Presolve removed 5565 rows and 42 columns
Presolve time: 0.14s
Presolved: 28038 rows, 22642 columns, 114268 nonzeros
Variable types: 212 continuous, 22430 integer (22430 binary)
Found heuristic solution: objective 2.2453760

Use crossover to convert LP symmetric solution to basic solution...

Root relaxation: objective 2.548987e+01, 194 iterations, 0.04 seconds (0.06 work units)

    Nodes    |    Current Node    |     Objective Bounds      |     Work
 Expl Unexpl |  Obj  Depth IntInf | Incumbent    BestBd   Gap | It/Node Time

     0     0   25.48987    0   23    2.24538   25.48987  1035%     -    0s
H    0     0                      18.5104500   25.48987  37.7%     -    0s
     0     0   25.39240    0   65   18.51045   25.39240  37.2%     -    0s
     0     0   25.39240    0   61   18.51045   25.39240  37.2%     -    0s
     0     0   25.39240    0  101   18.51045   25.39240  37.2%     -    0s
     0     0   25.29292    0  117   18.51045   25.29292  36.6%     -    0s
     0     0   25.29292    0   44   18.51045   25.29292  36.6%     -    1s
H    0     0                      20.3614950   25.29285  24.2%     -    1s
     0     0   25.29285    0   46   20.36149   25.29285  24.2%     -    1s
     0     0   25.29285    0   42   20.36149   25.29285  24.2%     -    1s
     0     0   25.29285    0   21   20.36149   25.29285  24.2%     -    2s
     0     0   25.29285    0   22   20.36149   25.29285  24.2%     -    2s
     0     0   25.29285    0   36   20.36149   25.29285  24.2%     -    2s
     0     0   25.29285    0   34   20.36149   25.29285  24.2%     -    2s
     0     0   25.29285    0   18   20.36149   25.29285  24.2%     -    3s
     0     0   25.29285    0   18   20.36149   25.29285  24.2%     -    3s
     0     0   25.29285    0   26   20.36149   25.29285  24.2%     -    3s
     0     0   25.29285    0   13   20.36149   25.29285  24.2%     -    3s
H    0     0                      20.7558260   25.29285  21.9%     -    4s
     0     2   25.29285    0   13   20.75583   25.29285  21.9%     -    4s
     7     9   25.28528    5   14   20.75583   25.28528  21.8%  59.0    5s
H  161    56                      21.1501570   25.28528  19.6%  14.2    9s
H  191    46                      22.3331500   25.28528  13.2%  13.9   10s
H  217    51                      22.7274810   25.28528  11.3%  13.3   10s
H  326    75                      23.1218120   25.28528  9.36%  13.0   14s
   330    77   25.28528   42   12   23.12181   25.28528  9.36%  13.0   15s
   497    93   25.28528   53   15   23.12181   25.28528  9.36%  13.3   20s
   628    82   24.76665   12  110   23.12181   25.28528  9.36%  15.1   25s
   785   155   25.28528   54   19   23.12181   25.28528  9.36%  14.4   30s

Cutting planes:
  Learned: 29
  Gomory: 5
  Cover: 157
  Implied bound: 92
  MIR: 318
  Inf proof: 6
  Zero half: 1
  Network: 1
  RLT: 1
  Relax-and-lift: 87

Explored 786 nodes (18231 simplex iterations) in 30.00 seconds (70.91 work units)
Thread count was 1 (of 192 available processors)

Solution count 9: 23.1218 22.7275 22.3332 ... 1.76176

Time limit reached
Best objective 2.312181200000e+01, best bound 2.528528100000e+01, gap 9.3568%
