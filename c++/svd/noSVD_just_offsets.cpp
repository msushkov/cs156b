#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cassert>

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <algorithm>

using namespace std;

// is sushkov using his mac or not?
const int MAC = 0;

// Filenames
const char dataFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data1234.txt";
const char dataFilePathUM[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data1234.txt";
const char outOfSampleFile[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data4.txt";
const char qualFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/allOnes.txt";

const char dataFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123.txt";
const char dataFilePathUMMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123.txt";
const char outOfSampleFileMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data4.txt";
const char qualFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/SVD_40f_min150e_K=0.02.txt";

// constants
const float GLOBAL_AVG_MOVIE_RATING = 3.60951619727;
const int NUM_USERS = 458293;
const int NUM_MOVIES = 17770;

float LEARNING_RATE_USER = 0.001;
float LEARNING_RATE_MOVIE = 0.001;
const float LEARNING_RATE_DECREASE_RATE = 1;

const float EPSILON = 0.0001;
const float VARIANCE_RATIO = 25;

// to be adjusted (and set by command line args)
int NUM_FEATURES;
float K;
int NUM_EPOCHS;

// defines a single training data point
class DataPoint {
public:
    int user;
    int movie;
    float rating;

    DataPoint() {
        this->user = 0;
        this->movie = 0;
        this->rating = 0;
    }

    DataPoint(unsigned int user, unsigned int movie, float rating) {
        this->user = user;
        this->movie = movie;
        this->rating = rating;
    }
};

// a vector of datapoints to hold data
vector<DataPoint*> *trainingData = new vector<DataPoint*>;

// feature matrices (2D arrays); feature k for user u is userFeatures[u][k]
float **userFeatures = new float*[NUM_USERS];
float **movieFeatures = new float*[NUM_MOVIES];

// arrays to hold the average ratings for each movie and for each user
float *avgUserRatings = new float[NUM_USERS];
float *avgMovieRatings = new float[NUM_MOVIES];

// the average offset for a given user
float *avgUserOffset = new float[NUM_USERS];
float *avgMovieOffset = new float[NUM_MOVIES];

// cache some products to get a bit of a speed boost when computing dot products in predictRating()
float *sumUpToFeatureK = new float[NUM_FEATURES];

// returns a random value between -0.01 and 0.01
float getRandom() {
	int num = ((int) rand()) % 100;
	return 2.0 * num / 10000.0 - 0.01;
}

// initialize each element of the feature matrices
void initialize(int numFeatures) {
    cout << "In initialize()" << endl;

    // initialize the user feature matrix
    for (int i = 0; i < NUM_USERS; i++)
        userFeatures[i] = new float[numFeatures];

    cout << "done1" << endl;

    for (int i = 0; i < NUM_USERS; i++) {
        for (int j = 0; j < numFeatures; j++) {
            //userFeatures[i][j] = 0.1;
            userFeatures[i][j] = sqrt(GLOBAL_AVG_MOVIE_RATING / (float) NUM_FEATURES) + getRandom();
        }
    }

    cout << "Done with user features..." << endl;
        
    // initialize the movie feature matrix
    for (int i = 0; i < NUM_MOVIES; i++)
        movieFeatures[i] = new float[numFeatures];
    for (int i = 0; i < NUM_MOVIES; i++) {
        for (int j = 0; j < numFeatures; j++) {
            //movieFeatures[i][j] = 0.1;
            movieFeatures[i][j] = sqrt(GLOBAL_AVG_MOVIE_RATING / (float) NUM_FEATURES) + getRandom();
        }
    }

    cout << "Done with movie features..." << endl;
}

// helper function that splits a string
vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

// helper function to split a string
vector<string> split(const string &s, char delim) {
    vector<string> elems;
    return split(s, delim, elems);
}

float predictRating_training(int user, int movie, float *movieFeatureVec, float *userFeatureVec, int numFeatures, int currFeature) {
    // at this point the features up to currFeature - 1 are trained and 
    // will not change for the current run of trainSVD()

    //float result = GLOBAL_AVG_MOVIE_RATING;
    float result = 0;

    if (currFeature >= 1) {
        sumUpToFeatureK[currFeature - 1] = userFeatureVec[currFeature - 1] * movieFeatureVec[currFeature - 1];

        if (currFeature > 1)
            sumUpToFeatureK[currFeature - 1] += sumUpToFeatureK[currFeature - 2];

        result += sumUpToFeatureK[currFeature - 1];
    }

    // calculate the dot product for the features that havent been trained yet in this round
    for (int f = currFeature; f < numFeatures; f++) {
        result += (userFeatureVec[f] * movieFeatureVec[f]);
    }

    return result;
}

float predictRating(int movie, int user, int numFeatures) {
	//float result = GLOBAL_AVG_MOVIE_RATING;
	//float result = userOffset + movieRating;
	float result = 0;

    // find the dot product
    for (int f = 0; f < numFeatures; f++) {
        result += (userFeatures[user][f] * movieFeatures[movie][f]);
    }

    return result;
}

// trains our algorithm given an input data point
void trainSVD(int user, int movie, float rating, int Kindex, float numFeatures) {
    float *userFeature = userFeatures[user - 1];
    float *movieFeature = movieFeatures[movie - 1];

    // iterate over features 0 through N - 1
    for (int k = 0; k < numFeatures; k++) {
        float error = rating - predictRating_training(user - 1, movie - 1, movieFeature, userFeature, numFeatures, k);
        //float error = rating - predictRating(movie - 1, user - 1, numFeatures);

        //float oldUserFeature = userFeature[k];
        //float oldMovieFeature = movieFeature[k];

        float oldUserFeature = userFeatures[user - 1][k];
        float oldMovieFeature = movieFeatures[movie - 1][k];

        // update user feature vector
        userFeatures[user - 1][k] += (LEARNING_RATE_USER * (error * oldMovieFeature - K * oldUserFeature));
        //userFeature[k] += (LEARNING_RATE_USER * (error * oldMovieFeature - K * oldUserFeature));

        // update movie feature vector
        movieFeatures[movie - 1][k] += (LEARNING_RATE_MOVIE * (error * oldUserFeature - K * oldMovieFeature));
        //movieFeature[k] += (LEARNING_RATE_MOVIE * (error * oldUserFeature - K * oldMovieFeature));

    } // end for loop over features
}

// compute the in-sample error
float computeInSample(int numFeatures) {
    float error = 0;

    // go through each training point and calculate the error for it
    for (int i = 0; i < trainingData->size(); i++) {
        int currUser = trainingData->at(i)->user;
        int currMovie = trainingData->at(i)->movie;
        float actualRating = trainingData->at(i)->rating;
        
        float predicted = predictRating(currMovie - 1, currUser - 1, numFeatures);

		// make sure the rating is between 1 and 5
        if (predicted > 5)
            predicted = 5;
        else if (predicted < 1)
            predicted = 1;

        error += (predicted - actualRating) * (predicted - actualRating);
    }

    return sqrt(error / trainingData->size());
}

// compute the out of sample error
float computeOutOfSample(int numFeatures) {
    float error = 0;
    int numPoints = 0;

    string line;
    ifstream outOfSample;
    
    if (MAC == 0)
    	outOfSample.open(outOfSampleFile, ios::in);
    else
    	outOfSample.open(outOfSampleFileMAC, ios::in);

    // go through the input data line by line
    while (getline(outOfSample, line)) {
        // where are we in the current line?
        int count = 0;

        int user = -1;
        int movie = -1;
        float actualRating = -1;

        istringstream lineIn(line);
        while (lineIn) {
            int val = 0;
            if (lineIn >> val) {
                if (count == 0)
                    user = val;
                else if (count == 1)
                    movie = val;
                else if (count == 3)
                    actualRating = val;
                count++;
            }
        }

        float predictedRating = predictRating(movie - 1, user - 1, numFeatures);

        // make sure the rating is between 1 and 5
        if (predictedRating > 5)
            predictedRating = 5;
        else if (predictedRating < 1)
            predictedRating = 1;
            
        assert(actualRating != -1);

        // get the squared error for this point
        error += (predictedRating - actualRating) * (predictedRating - actualRating);

        numPoints++;
    }    
    outOfSample.close();

    return sqrt(error / numPoints);
}

// iterate through the epochs and all the data points for each epoch
void learn() {
    float Eout = 10000;
    float prevEout = 1000000;

    initialize(NUM_FEATURES);

    int i = 0;

    // go N times through the data
    //for (int i = 0; i < NUM_EPOCHS; i++) {
    while (i < NUM_EPOCHS) { //|| (prevEout - Eout) > EPSILON) {
        //cout << "Current epoch" << "(feature " << f + 1 << ", K-value " << h + 1 << ") : " << i + 1 << " out of " << NUM_EPOCHS << endl;

        const clock_t begin_time = clock();

        // go through each point in the data set
        for (int p = 0; p < trainingData->size(); p++) {
            if (p % 30000000 == 0)
                cout << "Data point: " << p / 1000000 << " million" << endl;

            // train the SVD on each point (will go through all features)
            trainSVD(trainingData->at(p)->user, trainingData->at(p)->movie, 
                trainingData->at(p)->rating, K, NUM_FEATURES);

        } // end training data loop

        // randomize the training data
        next_permutation(trainingData->begin(), trainingData->end());

        // decrease the learning rates
        LEARNING_RATE_USER *= LEARNING_RATE_DECREASE_RATE;
        LEARNING_RATE_MOVIE *= LEARNING_RATE_DECREASE_RATE;

        // only check Eout every 5 iterations
        /*if (i % 3 == 0) {
            prevEout = Eout;
            Eout = computeOutOfSample(NUM_FEATURES);
            cout << "    Eout = " << Eout << endl;
        }*/

        cout << "Finished epoch: " << i + 1 << " out of " << NUM_EPOCHS << endl;
        cout << " --> " << float(clock() - begin_time) / CLOCKS_PER_SEC << " seconds" << endl;

        i++;

    } // end epoch loop

    // print out the results
    cout << endl << "RESULTS: (K, # of features, Ein, Eout)" << endl;
    cout << K << ", " << NUM_FEATURES << ", " << computeInSample(NUM_FEATURES) << ", " /*<< computeOutOfSample(NUM_FEATURES)*/ << endl;
}

// computes the average ratings for each user (uses UM data set)
// also computes the user offset
void getAvgUserRatings() {
    string line;

    // get the UM data file
    ifstream dataFile;
    if (MAC == 0)
    	dataFile.open(dataFilePathUM, ios::in);
    else
    	dataFile.open(dataFilePathUMMAC, ios::in);
    
    int numLinesSoFar = 0;

    int prevUser = 1;
    float totalRating = 0;
    int index = 0; // counts the # of movies each user rated

    float currTotalUserOffset = 0;

    // go through the input data line by line
    while (getline(dataFile, line)) {
        // where are we in the current line?
        int count = 0;

        int currUser = -1;
        int currMovie = -1;
        float currRating = -1;

        istringstream lineIn(line);
        while (lineIn) {
            int val = 0;
            if (lineIn >> val) {
                if (count == 0)
                    currUser = val;
                else if (count == 1)
                    currMovie = val;
                else if (count == 3)
                    currRating = val;
                count++;
            }
        }

        // to calculate the average ratings for the movies

        // if the user changes
        if (prevUser != currUser) {
            //avgUserRatings[prevUser - 1] = totalRating / index;
            avgUserRatings[prevUser - 1] = (GLOBAL_AVG_MOVIE_RATING * VARIANCE_RATIO + totalRating) / (VARIANCE_RATIO + index);
            totalRating = currRating;

            avgUserOffset[prevUser - 1] = currTotalUserOffset / index;
            currTotalUserOffset = currRating - avgMovieRatings[currMovie - 1];
            //currTotalUserOffset = currRating - GLOBAL_AVG_MOVIE_RATING;

            index = 1;
        } else { // if we keep seeing data for the same user
            if (currRating != 0) {
                index++;
                totalRating += currRating;
                currTotalUserOffset += (currRating - avgMovieRatings[currMovie - 1]);
                //currTotalUserOffset += (currRating - GLOBAL_AVG_MOVIE_RATING);
            }
        }

        prevUser = currUser;
            
        numLinesSoFar++;
        if (numLinesSoFar % 10000000 == 0)
            cout << "Computing avg user ratings: " << numLinesSoFar / 1000000 << " million" << endl;
    }    

    // deals with end of file
    avgUserRatings[prevUser - 1] = totalRating / index;
    avgUserOffset[prevUser - 1] = currTotalUserOffset / index;

    dataFile.close();
}

// reads data in (MU) and also computes avg ratings for each movie
// also computes the movie offset
void getData() {
    string line;
    ifstream dataFile;
    
    if (MAC)
    	dataFile.open(dataFilePathMAC, ios::in);
    else
    	dataFile.open(dataFilePath, ios::in);
    
    int numLinesSoFar = 0;

    int prevMovie = 1;
    float totalRating = 0;
    int index = 0; // counts the # of users that watched each movie

    float currTotalMovieOffset = 0;

    // go through the input data line by line
    while (getline(dataFile, line)) {
        // where are we in the current line?
        int count = 0;
        DataPoint *currPoint = new DataPoint();

        istringstream lineIn(line);
        while (lineIn) {
            int val = 0;
            if (lineIn >> val) {
                if (count == 0)
                    currPoint->user = val;
                else if (count == 1)
                    currPoint->movie = val;
                else if (count == 3)
                    currPoint->rating = val;
                count++;
            }
        }
        // add the point to our data vector
        trainingData->push_back(currPoint);

        // to calculate the average ratings for the movies
        if (prevMovie != currPoint->movie) {
            //avgMovieRatings[prevMovie - 1] = totalRating / index;
            avgMovieRatings[prevMovie - 1] = (VARIANCE_RATIO * GLOBAL_AVG_MOVIE_RATING + totalRating) / (VARIANCE_RATIO + index);
            
            totalRating = currPoint->rating;

            avgMovieOffset[prevMovie - 1] = currTotalMovieOffset / index;

            //currTotalMovieOffset = currPoint->rating - avgUserRatings[currPoint->user - 1];
            currTotalMovieOffset = currPoint->rating - GLOBAL_AVG_MOVIE_RATING;

            index = 1;
        } else {
            if (currPoint->rating != 0) {
                index++;
                totalRating += currPoint->rating;
                //currTotalMovieOffset += (currPoint->rating - avgUserRatings[currPoint->user - 1]);
                currTotalMovieOffset += (currPoint->rating - GLOBAL_AVG_MOVIE_RATING);
            }
        }

        prevMovie = currPoint->movie;
            
        numLinesSoFar++;
        if (numLinesSoFar % 10000000 == 0)
            cout << numLinesSoFar / 1000000 << " million" << endl;
    }    

    // deals with last line with respect to calculating the average movie rating
    //avgMovieRatings[prevMovie - 1] = totalRating / index;
    avgMovieRatings[prevMovie - 1] = (VARIANCE_RATIO * GLOBAL_AVG_MOVIE_RATING + totalRating) / (VARIANCE_RATIO + index);
    
    avgMovieOffset[prevMovie - 1] = currTotalMovieOffset / index;

    dataFile.close();

    // at this point we have the avg movie ratings filled out
    // now calculate the avg user ratings
    getAvgUserRatings();
}

// writes data out
void outputResults() {
    // output file
    ofstream outputFile;
    if (MAC)
    	outputFile.open(outputFilePathMAC, ios::out);
    else
    	outputFile.open(outputFilePath, ios::out);
    
    // qual file
    string line;
    ifstream qualFile;
    if (MAC)
    	qualFile.open(qualFilePathMAC, ios::in);
    else
    	qualFile.open(qualFilePath, ios::in);
    
    // go through each line of the qual file
    while (getline(qualFile, line)) {
        // where are we in the current line?
        int count = 0;

        int user = -1;
        int movie = -1;

        // read the values on the current line one by one
        istringstream lineIn(line);
        while (lineIn) {
            int val = 0;
            if (lineIn >> val) {
                if (count == 0)
                    user = val;
                else if (count == 1)
                    movie = val;
                count++;
            }
        }

        // calculate the rating for user, movie
        //float predictedScore = predictRating(movie - 1, user - 1, NUM_FEATURES);
        float predictedScore = 1;//avgMovieRatings[movie - 1] + avgUserOffset[user - 1];

        // make sure the rating is between 1 and 5
        /*if (predictedScore > 5)
            predictedScore = 5;
        else if (predictedScore < 1)
            predictedScore = 1;*/

        // write to file
        outputFile << predictedScore << endl;
    }

    qualFile.close();
    outputFile.close();
}

void cleanup() {
    for (int i = 0; i < NUM_USERS; i++)
        delete[] userFeatures[i];
        
    for (int i = 0; i < NUM_MOVIES; i++)
        delete[] movieFeatures[i];
        
    delete[] userFeatures;
    delete[] movieFeatures;

    cout << "Deleted user and movie features" << endl;

    for (int i = 0; i < trainingData->size(); i++)
        delete trainingData->at(i);
    delete trainingData;

    cout << "Deleted training data" << endl;

    delete[] avgMovieRatings;
    delete[] avgUserRatings;

    delete[] avgUserOffset;
    delete[] avgMovieOffset;

    delete[] sumUpToFeatureK;
}

// K-value, # features, # epochs
int main(int argc, char *argv[]) {
	srand((unsigned) time(0));

    // process the command line args
    if (argc != 4) {
        cout << "Usage: ./SVDadv [K-value] [# features] [# epochs]" << endl;
        return 1;
    }

    // process the command-line args
    K = atof(argv[1]);
    NUM_FEATURES = atoi(argv[2]);
    NUM_EPOCHS = atoi(argv[3]);

    //getData();
    cout << "Done with getData()" << endl;
    
    // run the SVD algorithm
    //learn();
    //cout << "Done learning" << endl;

    outputResults();
    cout << "Done with outputResults()" << endl;

    //cleanup();
    cout << "DONE!" << endl;
    
    return 0;
}