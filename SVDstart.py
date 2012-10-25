
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
NUM_USERS = 17000 # TODO: NEED EXACT VALUE
NUM_MOVIES = 500000 # TODO: NEED EXACT VALUE
LEARNING_RATE = 0.001

# initialize feature matrices (2D arrays); initialize to 0.1
# feature k for user u is userFeatures[k][u]
userFeatures = [[0.1] * NUM_USERS] * NUM_FEATURES
movieFeatures = [[0.1] * NUM_MOVIES] * NUM_FEATURES

# go through the input data line by line
for line in read_movie_file:
    # line: user_i movie_j date rating
    vals = line.split()
    
    curr_user = vals[0]
    curr_movie = vals[1]
    curr_rating = int(vals[3])
    
    # call the training method using the given data point
    trainSVD(curr_user, curr_movie, curr_rating)



# write out the results
for line in qual:
    vals = line.split()
    user = vals[0]
    movie = vals[1]
    
    # calculate the rating
    predicted_score = calculateScore(user, movie)
    
    # output the data to the submission file
    write_file.write(str(predicted_score) + '\n')

write_file.close()
qual.close()

# trains our algorithm given an input data point
def trainSVD(user, movie, rating):
    # TODO
    

# calculates the final score. this can only be done once we have
# trained on the data to get the feature vectors
def calculateScore(user, movie):
    # TODO