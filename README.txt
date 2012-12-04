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
11/11/12: Started to implement Koren's SVD++, but algorithm with y takes too long to converge so for now trying to get the version with the simple baselines working. Also trying to add caching in the dot product calculation.
11/12/12: Fixed a bug in SVD: movie and user offsets were not calculated using the global average.
11/15/12: 60 features, 120 epochs, K = 0.015, Lrate = 0.001: RMSEprobe = 0.930082, RMSEquiz = 0.93129.
11/15/12: Fixed a bug in caching of dot products. Ran the algorithm with 40 features, K = 0.02, Lrate = 0.001 for 150 epochs. Eout(probe) = 0.928722, Ein = 0.80932, RMSE(quiz) = 0.92985. Got to 2.26% above water.
11/19/12: Trying to implement a faster version of SVD in which the error only gets calculated once for every 10 features. Each epoch takes about 35 seconds, which is much faster than before.
11/23/12: Trying regular batch SVD with predicted rating as just the dot product; no offset or baseline. Feature vectors are initialized to sqrt(3.6... / # feat) + small random value between -0.01 and 0.01. Running with 40 features, K = 0.02, Lrate = 0.001, and 150 epochs. From iteration 135 to 138 Eout (measured on data4) went from 0.907295 to 0.907092. Training on data123. Final (after 150 iterations) Eout = 0.906555 (measured on data4), Ein = 0.748658, Quiz RMSE = 0.90818 (4.54% above water). (no training data randomization)
11/24/12: Ran SVD with 50 features, 157 iterations, K = 0.02, Lrate = 0.001. Ein (123) = 0.738339, Eout (4) = 0.905375, Quiz RMSE = 0.90706, 4.6605% above water (with data randomization). Put in probe into training after this, and trained on data1234, and got Quiz RMSE = 0.899 which is 5.51% above water.
11/26/12: Ran SVD with 100 features, 160 iterations, K = 0.02, Lrate = 0.001. Ein (123) = 0.615475, Eout (4) = 0.902798, Quiz RMSE = 0.90495, 4.8823% above water (with randomization). About 550 seconds per epoch.
11/27/12: Ran the 11/24 SVD with data1234 (threw probe into training). 50 features, 157 epochs, K = 0.02, Lrate = 0.001. Ein (1234) = 0.630875, Quiz RMSE = 0.899, 5.51% above water. About 270 seconds per epoch.
11/27/12: Also submitted a dumb solution where rating = avgMovieRatings[movie] + avgUserOffset[user] where the movie ratings are regularized. No learning, just some averages. Will try to aggregate this. Quiz RMSE = 0.98089, -3.1% above water.
11/28-29/12: Implementing SVD++. Cannot get an RMSE of lower than about 0.906 on data123. Tuning parameters...
12/1/12: Running current implementation of SVD++ with params specified in the Koren paper (all Lrates are 0.007, decrease by 0.9 every epoch, lambdas are 0.015 and 0.005). Ran for about 50 epochs and got an RMSE of about 0.912.
12/2/12: Ran a slightly modified SVD++ version (one feature loop rather than 2). Ran for 46 epochs, with 40 features, K = 0.015. Eout (on 4) = 0.913729.
12/2/12: Fixed bug in SVD++. Ran with 40 features, 34 iterations. Trained on data123, validated on data4. Parameters:
	double LEARNING_RATE_Y = 0.008;
	double LEARNING_RATE_USER = 0.008;
	double LEARNING_RATE_MOVIE = 0.008;
	double LRATE_ub = 0.008;
	double LRATE_mb = 0.008;

	const double K = 0.015;
	const double LAMBDA_Y = 0.015;
	const double LAMBDA_ub = 0.08;
	const double LAMBDA_mb = 0.08;

	Error progression:
		Eout = 0.990353
		Eout = 0.939368
		Eout = 0.923616
		Eout = 0.916893
		Eout = 0.913787
		Eout = 0.912242
		Eout = 0.911392
		Eout = 0.910883
		Eout = 0.91056
		Eout = 0.910349
		Eout = 0.91021
		Eout = 0.910117

	Ein = 0.743809, Eout (data4) = 0.910117.

Trained again, with new parameters:
	double LEARNING_RATE_Y = 0.007;
	double LEARNING_RATE_USER = 0.007;
	double LEARNING_RATE_MOVIE = 0.007;
	double LRATE_ub = 0.007;
	double LRATE_mb = 0.007;

	const double K = 0.015;
	const double LAMBDA_Y = 0.015;
	const double LAMBDA_ub = 0.005;
	const double LAMBDA_mb = 0.005;

	Ein = 0.0746881, Eout = 0.906643, Quiz RMSE = 0.90855.

Trained again, with parameters:
	./SVD++ 0.001 0.006 0.011 0.012 0.003 0.08 0.006 0.03 0.03 0.001

	Error progression:
		Eout = 0.982004
		Eout = 0.933086
		Eout = 0.918294
		Eout = 0.911471
		Eout = 0.908066
		Eout = 0.906281
		Eout = 0.90533
		Eout = 0.904816
		Eout = 0.904534
		Eout = 0.904376
		Eout = 0.904287

	50 features; Ein = 0.737636, Eout = 0.904287, Quiz RMSE = 0.90621.

Trained again, with y's initialized to random, feature vectors initialized to random, with norm = (N+1)^-0.5, with parameters:
	./SVD++ 0.0005 0.006 0.011 0.012 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.904585
	./SVD++ 0.001  0.006 0.011 0.012 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.904922
	./SVD++ 0.007  0.006 0.011 0.012 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.910962
	./SVD++ 0.003  0.006 0.011 0.012 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.906419
	./SVD++ 0.001  0.010 0.011 0.012 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.9072
	./SVD++ 0.001  0.003 0.011 0.012 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.907404
	./SVD++ 0.0005 0.006 0.013 0.012 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.90456
	./SVD++ 0.0005 0.006 0.011 0.012 0.005 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.904465
	./SVD++ 0.0005 0.005 0.012 0.012 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.904437
	./SVD++ 0.001  0.003 0.011 0.012 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.907404
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.904211
	./SVD++ 0.0005 0.006 0.01  0.01  0.001 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.905332
	./SVD++ 0.0005 0.006 0.008 0.008 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.904681
	./SVD++ 0.0005 0.004 0.01  0.012 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.90632
	./SVD++ 0.0003 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.904437
	./SVD++ 0.0005 0.006 0.011 0.009 0.001 0.07 0.005 0.02 0.02 0.0005 --> Eout = 
	./SVD++ 0.0005 0.001 0.011 0.009 0.003 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.928994

	./SVD++ 0.0005 0.006 0.011 0.012 0.003 0.07 0.005 0.02 0.02 0.01   --> Eout = 0.904883
	./SVD++ 0.0005 0.006 0.011 0.009 0.001 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.904965
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.02 0.001  --> Eout = 0.904324
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.01  0.02 0.02 0.0005 --> Eout = 0.908849
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.04 0.0005 --> Eout = 0.904063
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.04 0.02 0.0005 --> Eout = 0.904344
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.04 0.005 0.02 0.02 0.0005 --> Eout = 0.904566
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.007 0.02 0.04 0.0005 --> Eout = 0.904553
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.04 0.0007 --> Eout = 0.903918
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.09 0.005 0.02 0.04 0.0005 --> Eout = 0.904194
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.07 0.04 0.0005 --> Eout = 0.904185
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.06 0.0007 --> Eout = 0.903993
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.04 0.04 0.0007 --> Eout = 0.904142
	./SVD++ 0.0005 0.006 0.011 0.009 0.001 0.07 0.005 0.02 0.02 0.0005 --> Eout = 0.90512
	./SVD++ 0.0010 0.006 0.011 0.012 0.003 0.08 0.006 0.03 0.03 0      --> Eout = 0.904301
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.008 0.02 0.04 0.0007 --> Eout = 0.905603
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.08 0.005 0.02 0.06 0.0007 --> Eout = 0.903939
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.06 0.005 0.02 0.06 0.0007 --> Eout = 0.904115
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.09 0.005 0.02 0.06 0.0007 --> Eout = 0.904058
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.05 0.005 0.02 0.06 0.0007 --> Eout = 0.904171
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.06 0.001  --> Eout = 0.903914
 	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.06 0.007  --> Eout = 0.904185

80 features:
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.06 0.001 --> Eout = 0.902463

50 features with integrated model involving the w's and new Lrates = 0.001 and new LAMBDA = 0.015
	./SVD++_integrated 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.06 0.001

 (with currSumY[k] = totalUpdate):
 	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.06 0.001 --> Eout = 0.903993

From forum:	
	./SVD++ 0.001 0.006 0.011 0.012 0.003 0.08 0.006 0.03 0.03 0 --> Eout = 0.904301

With norm = N^-0.5:
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.04 0.0007 --> Eout = 0.904049 (better to do N+1)

With y's initialized to 0, norm = (N+1)^-0.5, feature vectors are random:
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.04 0.0007 --> Eout = 0.904129 (better to do y's to random)

With y's initialized to random, feature vectors initialized to sqrt(AVG / NUM_FEATURES) + random, predictRating doesnt use global avg:
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.04 0.0007 --> Eout = 0.918058

With result = avgUserRatings[user] + avgMovieOffset[movie] + userBias[user] + movieBias[movie]:
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.06 0.001 --> Eout = 0.905444

With result = avgUserOffset[user] + avgMovieRatings[movie] + userBias[user] + movieBias[movie]:
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.06 0.001 --> Eout = 0.907955

12/2/12: Submitted a solution that employed no learning and was just GLOBAL_AVG + userBias[user] + movieBias[movie]. RMSE = 0.97938. Biases were calculated using approach from BellKor's Advances in Coll. Filt. paper. Submitted another similar solution where rating was GLOBAL_AVG + userBias[user] + movieBias[movie] but biases were computed using the regularization in the Funk funny article. RMSE = 1.12461.

12/3/12: Need to aggregate solutions. Train all of them on data123_86%of4 and produce output for data_7%of4(aggr) and then do linear regression on all the files, using data_7%of4(aggr) are the r vector. This will give the weights for each solution. Then go back to the files for each individual solution that were trained on data1234 and aggregate them using the weights.

12/4/12: Tried using implicit ratings. 80 feature SVD++:
	./SVD++ 0.0005 0.006 0.011 0.009 0.003 0.07 0.005 0.02 0.06 0.001 --> Eout = 0.902394

90 feature SVD++ with implicit ratings: Quiz RMSE = 0.89807.

Variation of time SVD++ with 50 features: Quiz RMSE = 0.90055

Blended regularized baseline and kMeans. Quiz RMSE = 1.02244

SVD++ with implicit ratings and 90 features: Quiz RMSE = 0.89795

Tried aggregating solutions but files kept getting messed up so didn't make the 2pm deadline. In the end submitted the 5.75% AW SVD++ solution (150 features).

Done so far:
- Got the basic version of batch SVD to 4.66% above water (5.5% when training on data1234)
- Implemented k-Means clustering

Notes:
- RMSE (all 1's) = 2.90237
- RMSE (all 0's) = 3.84358

TODO:
- aggregate the 2.26% solution with the best regular SVD, and with a solution where
	predicted_rating = avgMovieRating[movie] + userOffset[movie] (regularized version)
	(maybe also throw the global average into that mix)
- finish SVD++
- aggregate/increment k-means with SVD (use pearson for the k-means)

algorithms:
- SVD (done)
- SVD++ (done)
- k means clustering (done)
- Boltzmann machines
- k nearest neighbors
- SimRank
