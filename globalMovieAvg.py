
read_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/all.dta', 'r')

numRatings = 0
totalRating = 0.0

for line in read_file:
	vals = line.split()
	rating = int(vals[3])

	if rating > 0:
		numRatings += 1
		totalRating += rating

		if numRatings % 1000000 == 0:
			print numRatings / 1000000

print "AVERAGE MOVIE RATING: " + str(totalRating / numRatings)

read_file.close()
