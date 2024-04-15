SCIP version 8.1.0 [precision: 8 byte] [memory: block] [mode: optimized] [LP solver: Soplex 6.0.4] [GitHash: 6129793871]
Copyright (c) 2002-2023 Zuse Institute Berlin (ZIB)

External libraries: 
  Soplex 6.0.4         Linear Programming Solver developed at Zuse Institute Berlin (soplex.zib.de) [GitHash: 950b1658]
  CppAD 20180000.0     Algorithmic Differentiation of C++ algorithms developed by B. Bell (github.com/coin-or/CppAD)
  ZLIB 1.2.11          General purpose compression library by J. Gailly and M. Adler (zlib.net)
  GMP 6.2.1            GNU Multiple Precision Arithmetic Library developed by T. Granlund (gmplib.org)
  ZIMPL 3.5.3          Zuse Institute Mathematical Programming Language developed by T. Koch (zimpl.zib.de)
  AMPL/MP 4e2d45c4     AMPL .nl file reader library (github.com/ampl/mp)
  PaPILO 2.1.4         parallel presolve for integer and linear optimization (github.com/scipopt/papilo) [GitHash: ee0677c4]
  bliss 0.77           Computing Graph Automorphisms by T. Junttila and P. Kaski (users.aalto.fi/~tjunttil/bliss/)
  sassy 1.1            Symmetry preprocessor by Markus Anders (github.com/markusa4/sassy)
  Ipopt 3.13.2         Interior Point Optimizer developed by A. Waechter et.al. (github.com/coin-or/Ipopt)

user parameter file <scip.set> not found - using default parameters


read problem </pub/netdisk1/linpeng/Local-MIP/test/2dbpp.mps>
============

original problem has 22684 variables (22472 bin, 0 int, 0 impl, 212 cont) and 33603 constraints
limits/time = 30

feasible solution found by trivial heuristic after 0.1 seconds, objective value 0.000000e+00
presolving:
(round 1, fast)       42 del vars, 0 del conss, 0 add conss, 0 chg bounds, 212 chg sides, 212 chg coeffs, 0 upgd conss, 0 impls, 0 clqs
   (0.1s) running MILP presolver
   (0.2s) MILP presolver (2 rounds): 0 aggregations, 0 fixings, 0 bound changes
(round 2, medium)     42 del vars, 33603 del conss, 28038 add conss, 0 chg bounds, 212 chg sides, 212 chg coeffs, 0 upgd conss, 0 impls, 0 clqs
(round 3, exhaustive) 42 del vars, 33603 del conss, 28038 add conss, 0 chg bounds, 212 chg sides, 212 chg coeffs, 5649 upgd conss, 0 impls, 0 clqs
   (0.7s) probing: 1000/22430 (4.5%) - 0 fixings, 0 aggregations, 2485 implications, 0 bound changes
   (0.7s) probing: 1001/22430 (4.5%) - 0 fixings, 0 aggregations, 2488 implications, 0 bound changes
   (0.7s) probing aborted: 1000/1000 successive useless probings
   (0.9s) symmetry computation started: requiring (bin +, int -, cont +), (fixed: bin -, int +, cont -)
   (1.3s) symmetry computation finished: 97 generators found (max: 1500, log10 of symmetry group size: 106.7) (symcode time: 0.42)
(round 4, exhaustive) 42 del vars, 33603 del conss, 28391 add conss, 0 chg bounds, 212 chg sides, 212 chg coeffs, 5649 upgd conss, 2084 impls, 488 clqs
presolving (5 rounds: 5 fast, 4 medium, 3 exhaustive):
 42 deleted vars, 33603 deleted constraints, 28391 added constraints, 0 tightened bounds, 0 added holes, 212 changed sides, 212 changed coefficients
 2084 implications, 488 cliques
presolved problem has 22642 variables (22430 bin, 0 int, 0 impl, 212 cont) and 28391 constraints
     84 constraints of type <varbound>
  22742 constraints of type <linear>
   5565 constraints of type <logicor>
Presolving Time: 1.46
transformed 1/1 original solutions to the transformed problem space

 time | node  | left  |LP iter|LP it/n|mem/heur|mdpt |vars |cons |rows |cuts |sepa|confs|strbr|  dualbound   | primalbound  |  gap   | compl. 
i 1.6s|     1 |     0 |     0 |     - |  oneopt|   0 |  22k|  28k|  28k|   0 |  0 |   0 |   0 | 1.892453e+02 | 2.245376e+00 |8328.23%| unknown
  1.7s|     1 |     0 |   409 |     - |   419M |   0 |  22k|  28k|  28k|   0 |  0 |   0 |   0 | 2.548987e+01 | 2.245376e+00 |1035.22%| unknown
  1.9s|     1 |     0 |   608 |     - |   425M |   0 |  22k|  28k|  28k|   9 |  1 |   0 |   0 | 2.539877e+01 | 2.245376e+00 |1031.16%| unknown
  2.2s|     1 |     0 |   702 |     - |   429M |   0 |  22k|  28k|  28k|  17 |  2 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
  2.5s|     1 |     0 |   776 |     - |   434M |   0 |  22k|  28k|  28k|  31 |  3 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
  2.8s|     1 |     0 |   894 |     - |   442M |   0 |  22k|  28k|  28k|  48 |  4 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
  3.2s|     1 |     0 |  1106 |     - |   447M |   0 |  22k|  28k|  28k|  62 |  5 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
  3.5s|     1 |     0 |  1261 |     - |   456M |   0 |  22k|  28k|  28k|  71 |  6 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
  3.8s|     1 |     0 |  1454 |     - |   462M |   0 |  22k|  28k|  28k|  84 |  7 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
  4.1s|     1 |     0 |  1609 |     - |   470M |   0 |  22k|  28k|  28k| 103 |  8 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
  4.4s|     1 |     0 |  1772 |     - |   473M |   0 |  22k|  28k|  28k| 122 |  9 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
  4.8s|     1 |     0 |  1856 |     - |   482M |   0 |  22k|  28k|  28k| 132 | 10 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
  4.9s|     1 |     0 |  1991 |     - |   485M |   0 |  22k|  28k|  28k| 147 | 11 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
  5.1s|     1 |     0 |  2114 |     - |   488M |   0 |  22k|  28k|  28k| 170 | 12 |   0 |   0 | 2.539240e+01 | 2.245376e+00 |1030.88%| unknown
L 6.9s|     1 |     0 |  9149 |     - |    rens|   0 |  22k|  28k|  28k| 170 | 12 |   0 |   0 | 2.539240e+01 | 4.007133e+00 | 533.68%| unknown
 time | node  | left  |LP iter|LP it/n|mem/heur|mdpt |vars |cons |rows |cuts |sepa|confs|strbr|  dualbound   | primalbound  |  gap   | compl. 
L 7.1s|     1 |     0 |  9149 |     - |    alns|   0 |  22k|  28k|  28k| 170 | 12 |   0 |   0 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
  7.1s|     1 |     0 |  9149 |     - |   492M |   0 |  22k|  28k|  28k| 170 | 12 |   0 |   0 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
  7.2s|     1 |     0 |  9402 |     - |   495M |   0 |  22k|  28k|  28k| 186 | 13 |   0 |   0 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
  7.3s|     1 |     0 |  9469 |     - |   495M |   0 |  22k|  28k|  28k| 193 | 14 |   0 |   0 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
  8.6s|     1 |     0 |  9844 |     - |   497M |   0 |  22k|  28k|  28k| 193 | 14 |   0 |  28 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
  8.7s|     1 |     0 |  9951 |     - |   500M |   0 |  22k|  28k|  28k| 212 | 15 |   0 |  28 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
  8.7s|     1 |     0 | 10041 |     - |   500M |   0 |  22k|  28k|  28k| 218 | 16 |   0 |  28 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
 10.2s|     1 |     0 | 10186 |     - |   500M |   0 |  22k|  28k|  28k| 218 | 16 |   0 |  61 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
 10.3s|     1 |     0 | 10441 |     - |   503M |   0 |  22k|  28k|  28k| 230 | 17 |   0 |  61 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
 10.3s|     1 |     0 | 10559 |     - |   503M |   0 |  22k|  28k|  28k| 240 | 18 |   0 |  61 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
 11.3s|     1 |     2 | 10647 |     - |   503M |   0 |  22k|  28k|  28k| 240 | 18 |   0 |  79 | 2.539240e+01 | 4.693206e+00 | 441.05%| unknown
L14.2s|    39 |    32 | 21331 | 471.6 |    alns|  23 |  22k|  28k|  28k| 361 |  2 |   2 |  79 | 2.539240e+01 | 5.714667e+00 | 344.34%| unknown
L14.4s|    39 |    32 | 21331 | 471.6 |    alns|  23 |  22k|  28k|  28k| 361 |  2 |   2 |  79 | 2.539240e+01 | 7.404180e+00 | 242.95%| unknown
L15.3s|    39 |    32 | 21331 | 471.6 |    alns|  23 |  22k|  28k|  28k| 361 |  2 |   2 |  79 | 2.539240e+01 | 7.565712e+00 | 235.62%| unknown
 20.1s|   100 |    75 | 40994 | 379.6 |   545M |  23 |  22k|  28k|  28k| 450 |  1 |   4 |  79 | 2.539240e+01 | 7.565712e+00 | 235.62%|   5.89%
 time | node  | left  |LP iter|LP it/n|mem/heur|mdpt |vars |cons |rows |cuts |sepa|confs|strbr|  dualbound   | primalbound  |  gap   | compl. 
 28.7s|   200 |   147 | 93444 | 452.4 |   553M |  39 |  22k|  28k|  28k| 587 |  1 |  20 |  79 | 2.539240e+01 | 7.565712e+00 | 235.62%|   8.98%

SCIP Status        : solving was interrupted [time limit reached]
Solving Time (sec) : 30.01
Solving Nodes      : 230
Primal Bound       : +7.56571200000000e+00 (7 solutions)
Dual Bound         : +2.53924013651873e+01
Gap                : 235.62 %


