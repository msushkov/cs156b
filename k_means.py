import random
import numpy as np

sushkov = False

if sushkov:
    read_movie_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data1.txt', 'r')
    read_user_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data1.txt', 'r')
    qual = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta', 'r')
    write_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVDstart.txt', 'w')
else:
    read_movie_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\data2.txt', 'r')
    read_user_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\data2.txt', 'r')
    qual = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\qual.dta', 'r')
    write_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\k_means.txt', 'w')

##############################################
# HELPER METHODS
##############################################

# constants
#NUM_FEATURES = 10
#NUM_USERS = 458293
#NUM_MOVIES = 17770
#LEARNING_RATE = 0.001
#K = 1000 # here K is the number of clusters, optimize
#EPSILON = 10^-3


first = True
# train kMeans
def kMeans(allFeatVectors, K):
    
    first = True
    assignedKs = 1
    previous_assignedKs = 0
    
    while assignedKs != previous_assignedKs:
        if not first:
            previous_assignedKs = assignedKs[:]
        assignedKs = []
        
        for featVector in allFeatVectors:
        
            if first:    
                # initialize the Ks
                clusterCenters = [[0] * len(featVector)] * K
    
                # randomly assign starting values for the clusters
                # for i in range(len(clusterCenters)):
                for i in range(K):
                    clusterCenters[i] = [random.random() for feature in featVector]
                    #DELETE clusterCenters[i] = allFeatVectors[i]
                first = False
        
                assert(len(featVector) == len(clusterCenters[0]))
    
            # calculate Euclidian distance
            distance = [999999] * len(featVector)
            # going through the K's that I have
            for l, center in enumerate(clusterCenters): # there are K clusterCenters of length len(featVector)
                cum_sum = 0
                for ind in range(len(featVector)):
                    cum_sum += (featVector[ind] - center[ind]) ** 2
                # distance is the distance that each point is away from the cluster mean
                distance[l] = (cum_sum ** .5)
                # center that a certain featVector was assigned to
                assignedK = distance.index(min(distance))
        
            # list of all the assignedKs for the next iterations
            assignedKs.append(assignedK)
            
        # Update the K values
        for i in range(K):
            agg_sum = 0
            counter = 0
            for val in assignedKs:
                if val == i:
                    agg_sum += np.array(clusterCenters[i])
                    counter += 1
            try:
                clusterCenters[i] = list(agg_sum / counter)
            except:
                # If none of the points were categorized as part of clusterCenters[i] that time
                clusterCenters[i] = [random.random() for feature in featVector]
                continue
        
    # Returns the groups    
    return assignedKs


#Testing a few things
allFeatVectors = [[0.1, 0.5, 0.3], [0.2, 0.8, 0.23], [0.253, 0.81, 0.3], [0.8, 1, 0]]
featVector = allFeatVectors[0]
K = 3 #K will have groups of range(K)
clusterCenters = [[0] * len(featVector)] * K
for i in range(len(clusterCenters)):
    clusterCenters[i] = [random.random() for feature in featVector]
center0 = clusterCenters[0]
center1 = clusterCenters[1]
center2 = clusterCenters[2]
distance = [999999] * len(featVector)
cum_sum = 0

read_movie_file.close()
read_user_file.close()
qual.close()
write_file.close()