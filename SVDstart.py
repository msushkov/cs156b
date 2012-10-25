
sushkov = True

if sushkov:
    read_movie_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data1.txt', 'r')
    read_user_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data1.txt', 'r')
    qual = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta', 'r')
    write_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVDstart.txt', 'w')
else:
    read_movie_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\data2.txt', 'r')
    read_user_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data2.txt', 'r')
    qual = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\qual.dta', 'r')
    write_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\sub2\average_of_averages.txt', 'w')

# constants
NUM_FEATURES = 40
NUM_USERS = 458293
NUM_MOVIES = 17770
LEARNING_RATE = 0.001
K = 25
EPSILON = 10^-5

# initialize feature matrices (2D arrays); initialize to 0.1
# feature k for user u is userFeatures[k][u]
userFeatures = [[0.1] * NUM_USERS] * NUM_FEATURES
movieFeatures = [[0.1] * NUM_MOVIES] * NUM_FEATURES

numLinesSoFar = 0

# go through the input data line by line
for line in read_movie_file:
    # line: user_i movie_j date rating
    vals = line.split()
    
    curr_user = vals[0]
    curr_movie = vals[1]
    curr_rating = int(vals[3])
    
    # call the training method using the given data point
    trainSVD(curr_user, curr_movie, curr_rating)
    
    numLinesSoFar += 1
    
    # print every 1000 lines
    if numLinesSoFar % 1000 == 0:
        print numLinesSoFar

# at this point we have userFeatures and movieFeatures completely populated

# write out the results
for line in qual:
    vals = line.split()
    user = vals[0]
    movie = vals[1]
    
    # calculate the rating
    predicted_score = calculateScore(user, movie)
    
    # output the data to the submission file
    write_file.write(str(predicted_score) + '\n')

# close the files
write_file.close()
qual.close()

# HELPER FUNCTIONS

# trains our algorithm given an input data point
def trainSVD(user, movie, rating):
    numIterations = 0

    # large so we go into the first iteration of the while loop
    oldError = 10^9
    assert(abs(LEARNING_RATE * (rating - predictRating(movie, user)) - oldError) > EPSILON)
    
    # learn while our error is decreasing by an amount higher than epsilon
    # or until we hit 200 iterations
    while abs(LEARNING_RATE * (rating - predictRating(movie, user)) - error) > EPSILON or numIterations < 200:
        # calculate the new error
        error = LEARNING_RATE * (rating - predictRating(movie, user))
    
        # iterate over features 0 through 39
        for k in range(NUM_FEATURES):
            oldUserFeature = userFeatures[k][user]
            userFeatures[k][user] += LEARNING_RATE * (error * movieFeatures[k][movie] - K * oldUserFeature)
            movieFeatures[k][movie] += LEARNING_RATE * (error * oldUserFeature - K * movieFeatures[k][movie])
            
        numIterations += 1
        
# calculates the dot product of the feature vectors for a given user and movie
def predictRating(movie, user):
    return dotProduct(userFeatures[:][user], movieFeatures[:][movie]) 
    
# calculate the dot product of 2 lists
def dotProduct(list1, list2):
    assert(len(list1) == len(list2))
    
    result = 0.0
    for i in range(len(list1)):
        result += (float(list1[i]) * float(list2[i]))
    return result
        
        