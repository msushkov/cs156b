
sushkov = True

if sushkov:
    read_movie_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data1.txt', 'r')
    qual = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta', 'r')
    write_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVDstart3.txt', 'w')
else:
    pass
    #read_movie_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\data2.txt', 'r')
    #read_user_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data2.txt', 'r')
    #qual = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\qual.dta', 'r')
    #write_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\sub2\average_of_averages.txt', 'w')

NUM_USERS = 458293
NUM_MOVIES = 17770

NUM_FEATURES = 5
LEARNING_RATE = 0.001
K = 0.02
EPSILON = 10^-5
MAX_ITERATIONS = 1
NUM_EPOCHS = 1

class DataPoint:
    def __init__(self, user, movie, rating):
        self.user = user
        self.movie = movie
        self.rating = rating

# a list of datapoints to hold data
trainingData = []

# feature matrices (2D arrays)
# feature k for user u is userFeatures[u][k]
userFeatures = []
movieFeatures = []

# initialize each element of the feature matrices to 0.1
def initialize():
    # initialize the user feature matrix to all 0.1's
    cdef int i
    for i in range(NUM_USERS):
        userFeatures.append([0.1] * NUM_FEATURES)

    for i in range(NUM_MOVIES):
        movieFeatures.append([0.1] * NUM_FEATURES)

# calculates the dot product of the feature vectors for a given user and movie
cpdef double predictRating(int movie, int user):
    assert user < NUM_USERS
    assert user >= 0
    assert movie >= 0
    assert movie < NUM_MOVIES

    cdef int i
    cdef double result = 0.0
    for i in range(NUM_FEATURES):
        result += (userFeatures[user][i]) * (movieFeatures[movie][i])

    return result

# trains our algorithm given an input data point
def trainSVD(user, movie, rating):
    assert user <= NUM_USERS
    assert user > 0
    assert movie > 0
    assert rating > 0
    assert rating <= 5
    assert movie <= NUM_MOVIES

    cdef int k
    for k in range(NUM_FEATURES):
        error = rating - predictRating(movie - 1, user - 1)

        oldUserFeature = userFeatures[user - 1][k];
        userFeatures[user - 1][k] += LEARNING_RATE * (error * movieFeatures[movie - 1][k] - K * oldUserFeature)
        movieFeatures[movie - 1][k] += LEARNING_RATE * (error * oldUserFeature - K * movieFeatures[movie - 1][k])    

# iterate through the epochs and all the data points for each epoch
def learn():
    cdef int i
    cdef int p
    cdef int size
    # go N times through the data
    for i in range(NUM_EPOCHS):
        print "Current epoch: " + str(i + 1) + " out of " + str(NUM_EPOCHS)

        # go through each point in the data set
        for p in range(len(trainingData)):
            trainSVD(trainingData[p].user, trainingData[p].movie, trainingData[p].rating)



print 'Starting...'

numLinesSoFar = 0

# go through the input data line by line
for line in read_movie_file:
    # line: user_i movie_j date rating
    vals = line.split()
    
    curr_user = int(vals[0])
    curr_movie = int(vals[1])
    curr_rating = int(vals[3])
    
    currPt = DataPoint(curr_user, curr_movie, curr_rating)
    trainingData.append(currPt)
    
    numLinesSoFar += 1
    if numLinesSoFar % 1000000 == 0:
        print numLinesSoFar

learn()

# write out the results
for line in qual:
    vals = line.split()
    user = int(vals[0])
    movie = int(vals[1])
    
    # calculate the rating
    predicted_score = predictRating(user, movie)
    
    # output the data to the submission file
    write_file.write(str(predicted_score) + '\n')

# close the files
write_file.close()
qual.close()    

print 'DONE!'