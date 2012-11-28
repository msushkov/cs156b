
SVDfile = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/SVD_data1234_50f_157e_K=0.02(5.51%AW).txt";
offsetFile = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/noSVD_only_offsets.txt";

g1sq = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/SVD_5.51_squared.txt";
g2sq = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/offsets_squared.txt";

data1 = open(SVDfile, 'r')
data2 = open(offsetFile, 'r')

g1 = open(g1sq, 'w')
g2 = open(g2sq, 'w')

sumg1 = 0.0
sumg2 = 0.0
sumg1sq = 0.0
sumg2sq = 0.0
g1g2 = [0.0] * 2749898

for i, line in enumerate(data1):
	val = float(line)
	sumg1 += val
	sumg1sq += val * val
	g1g2[i] = val
	g1.write(str(val * val) + "\n")

for i, line in enumerate(data2):
	val = float(line)
	sumg2 += val
	sumg2sq += val * val
	g1g2[i] = val * g1g2[i]
	g2.write(str(val * val) + "\n")

print "sum g1g2 = " + str(sum(g1g2))
print "sum g1 = " + str(sumg1)
print "sum g2 = " + str(sumg2)
print "sum g1sq = " + str(sumg1sq)
print "sum g2sq = " + str(sumg2sq)

data1.close()
data2.close()
g1.close()
g2.close()

