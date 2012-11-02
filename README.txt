cs156b
======

brogrammers

10/16/12: Wrote some code to split all.dta into the 5 data sets specified by all.idx
10/17/12: Implementing average of averages
10/17/12: Ran average of averages and submitted to scoreboard. Currently at -6% compared to water level.
10/25/12: Implemented basic version of SVD. Will take a month to run in python so will try to code in c++ and need to think of optimizations.
10/27/12 - 10/29/12: Have been working on basic C++ implementation of SVD. Had a lot of bugs and seg faults but finally fixed that on 10/29/12. Currently getting 300% below water so need to fix algorithm, but at least there are no seg faults...
10/29/12: Finished basic SVD implementation in C++ (bugs and seg faults fixed). Submitted a solution with K = 0.001, 15 features, and 80 epochs. Took about 2 hours to train and got 1.13% above water.
10/30/12: Added "better mean" and offset calculations for SVD. Also added functionality to calculate Ein and Eout.
10/31/12: Ran the new SVD with K = 0.02, 150 epochs, and 30 features and got 1.9% above water. Ein = 0.81775 (data1) and Eout = 0.84188 (data2). Quiz RMSE = 0.93321.
11/1/12: Added ratio of variances code and ran the new SVD 6 times to see how many epochs gives the best Eout (calculated Eout on data2, trained on data1). All runs had K = 0.02 and had 30 features. 
	15 epochs: Ein = 0.907, Eout = 0.91276 
	25 epochs: Ein = 0.8865, Eout = 0.894 
	40 epochs: Ein = 0.8795, Eout = 0.888
	70 epochs: Ein = 0.857, Eout = 0.8696
	100 epochs:  Ein = 0.8438, Eout = 0.86
	120 epochs: Ein = 0.8343, Eout = 0.853

TODO: 
- refactor code and make a library for loading data
- write aggregation code (python)
- add a baseline predicted rating into the SVD (use averages)

algorithms:
- Boltzmann machines
- k nearest neighbors
- SimRank
- k means clustering
