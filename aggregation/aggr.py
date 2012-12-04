import numpy as np
import numpy.linalg as la

# the aggregation set
fileAggr = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data_7%of4(aggr).txt"
NUM_LINES = 96305 # in aggr
NUM_LINES_QUAL = 2749898 # in qual

### TO BE ADJUSTED

NUM_SOLUTIONS = 3

# the files from our algorithms that were trained on data123_86%of4
#file1 = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/k_means/kMeans_Qual.txt" # kmeans
#file2 = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/no_learning/regularizedBaseline_qual.txt" # baseline
file3 = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD++/SVD++_40f_10e_123_86%of4_qual(4.76%AW).txt" # svd++
file4 = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD++/timeSVD++_30f_123_86%of4_qual(5.11%AW).txt" # timeSVD++
#file5 = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/svd/new/SVD_30f_qual.txt" # regular svd
#file6 = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD++/timeSVD++_20f_123_86%of4_qual.txt"
#file7 = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD++/SVD++_150f_123_86%of4(5.75%AW)_qual.txt"

# the files that were trained on the aggregation set
#file1Aggr = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/k_means/kMeans_Aggregate.txt" # kmeans
#file2Aggr = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/baselines/regularizedBaseline_aggr.txt" # baseline
file3Aggr = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/svd++/SVD++_40f_10e_aggr.txt" # svd++
file4Aggr = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/svd++/timeSVD++_30f_aggr.txt" # timeSVD++
#file5Aggr = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/svd/SVD_30f_onAggrSet.txt" # regular svd
#file6Aggr = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/svd++/timeSVD++_20f_aggr.txt"
#file7Aggr = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/svd++/SVD++_150f_aggr.txt"

# output file SVD30f157e
fileout = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/3solutions.txt"

### AGGREGATION

G = np.zeros((NUM_LINES, NUM_SOLUTIONS)) # matrix with our solutions
w = None
ratings = np.zeros((NUM_LINES_QUAL, NUM_SOLUTIONS))

### populate G: MAY NEED TO ADD STUFF BELOW

# data1 = open(file1Aggr, 'r')
# for i, line in enumerate(data1):
# 	G[i][0] = float(line.strip())
# data1.close()

# data2 = open(file2Aggr, 'r')
# for i, line in enumerate(data2):
# 	G[i][1] = float(line)
# data2.close()

data3 = open(file3Aggr, 'r')
for i, line in enumerate(data3):
	G[i][0] = float(line)
data3.close()

data4 = open(file4Aggr, 'r')
for i, line in enumerate(data4):
	G[i][1] = float(line)
data4.close()

# data5 = open(file5Aggr, 'r')
# for i, line in enumerate(data5):
# 	G[i][4] = float(line)
# data5.close()

#######

# get the r vector (the actual ratings on the aggregation set)
r = np.zeros(NUM_LINES)
dataAggr = open(fileAggr, 'r')
for i, line in enumerate(dataAggr):
	vals = line.split()
	r[i] = float(vals[3])
dataAggr.close()

# get the w's
w = np.dot(la.pinv(G), r)

##### MAY NEED TO ADD STUFF BELOW

# dataQual1 = open(file1, 'r')
# for i, line in enumerate(dataQual1):
# 	ratings[i][0] = float(line.rstrip())
# dataQual1.close()

# dataQual2 = open(file2, 'r')
# for i, line in enumerate(dataQual2):
# 	ratings[i][0] = float(line.rstrip())
# dataQual2.close()

dataQual3 = open(file3, 'r')
for i, line in enumerate(dataQual3):
	ratings[i][0] = float(line.rstrip())
dataQual3.close()

dataQual4 = open(file4, 'r')
for i, line in enumerate(dataQual4):
	ratings[i][1] = float(line.rstrip())
dataQual4.close()

# dataQual5 = open(file5, 'r')
# for i, line in enumerate(dataQual5):
# 	ratings[i][4] = float(line.rstrip())
# dataQual5.close()


# no need to modify
out = open(fileout, 'w')
for lst in ratings:
	newRating = np.dot(w, lst)

	if newRating < 1:
		newRating = 1
	elif newRating > 5:
		newRating = 5

	out.write(str(newRating) + "\n")
out.close()