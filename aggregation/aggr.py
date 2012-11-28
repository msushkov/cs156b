# aggregates 2 solutions using quiz as a validation set

file2 = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/noSVD_only_offsets.txt";
file1 = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/SVD_data1234_50f_157e_K=0.02(5.51%AW).txt";
file3 = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/SVD_5.51_and_offsets.txt";

w0 = -557.553
w1 = 28.3022
w2 = 124.533

NUM_LINES = 2749898

def calculateRating(val1, val2):
	return w0 + val1 * w1 + val2 * w2

data1 = open(file1, 'r')
data2 = open(file2, 'r')
out = open(file3, 'w')

ratings = [0.0] * NUM_LINES

for i, line in enumerate(data1):
	val = float(line)
	ratings[i] = val

for i, line in enumerate(data2):
	val = float(line)
	ratings[i] = calculateRating(val, ratings[i])

for r in ratings:
	if r < 1:
		r = 1
	elif r > 5:
		r = 5
	print r
	out.write(str(r) + "\n")

data1.close()
data2.close()