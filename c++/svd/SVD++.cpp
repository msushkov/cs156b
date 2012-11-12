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

using namespace std;

// is sushkov using his mac or not?
const int MAC = 0;

// Filenames
const char dataFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data1.txt";
const char dataFilePathUM[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data1.txt";
const char outOfSampleFile[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data4.txt";
const char qualFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/SVD++_initial.txt";

const char dataFilePathMac[] = "/users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data2.txt";
const char qualFilePathMac[] = "/users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePathMac[] = "/users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/SVDstart.txt";

// constants
const double GLOBAL_AVG_MOVIE_RATING = 3.60951619727;
const int NUM_USERS = 458293;
const int NUM_MOVIES = 17770;
double LEARNING_RATE = 0.001;
const int EPSILON = 10^(-4);
const int LENGTH_NUM_FEATURES = 1;
const int LENGTH_K = 1;
const double LEARNING_RATE_DECREASE_RATE = 0.9;

// to be adjusted (and set by command line args)
int NUM_FEATURES[LENGTH_NUM_FEATURES];
double K[LENGTH_K];
int NUM_EPOCHS;

// defines a single training data point
class DataPoint {
public:
    int user;
    int movie;
    int rating;

    DataPoint() {
        this->user = 0;
        this->movie = 0;
        this->rating = 0;
    }

    DataPoint(int user, int movie, int rating) {
        this->user = user;
        this->movie = movie;
        this->rating = rating;
    }
};

// a vector of datapoints to hold data
vector<DataPoint*> *trainingData = new vector<DataPoint*>;

// feature matrices (2D arrays); feature k for user u is userFeatures[u][k]
double **userFeatures = new double*[NUM_USERS];
double **movieFeatures = new double*[NUM_MOVIES];

// arrays to hold the average ratings for each movie and for each user
double *avgUserRatings = new double[NUM_USERS];
double *avgMovieRatings = new double[NUM_MOVIES];

// the average offset for a given user
double *avgUserOffset = new double[NUM_USERS];
double *avgMovieOffset = new double[NUM_MOVIES];

// cache some products to get a bit of a speed boost when computing dot products in predictRating()
double *sumUpToFeatureK = new double[NUM_FEATURES[0]];

// b_u and b_i
//double *userBias = new double[NUM_USERS];
//double *movieBias = new double[NUM_MOVIES];

// For each user, the number of movies they rated
int *movieRatingsByUser = new int[NUM_USERS];

// stores the y-values of each y[user][feature]
double **y = new double*[NUM_USERS];

// initialize each element of the feature matrices to 0.1
void initialize(int numFeatures) {
    // initialize the user feature matrix to all 0.1's
    for (int i = 0; i < NUM_USERS; i++)
        userFeatures[i] = new double[numFeatures];
    for (int i = 0; i < NUM_USERS; i++) {
        for (int j = 0; j < numFeatures; j++)
            userFeatures[i][j] = 0.1;
    }
        
    // initialize the movie feature matrix to all 0.1's
    for (int i = 0; i < NUM_MOVIES; i++)
        movieFeatures[i] = new double[numFeatures];
    for (int i = 0; i < NUM_MOVIES; i++) {
        for (int j = 0; j < numFeatures; j++)
            movieFeatures[i][j] = 0.1;
    }   

    // initialize y
    for (int i = 0; i < NUM_USERS; i++)
        y[i] = new double[numFeatures];
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

// calculates a single product for a given user, movie, and feature
inline double calculateProduct(int user, int movie, int f) {
    //int numMoviesRated = movieRatingsByUser[user];

    //if (numMoviesRated == 0)
        return movieFeatures[movie][f] * userFeatures[user][f];
    //else
        //return movieFeatures[movie][f] * (userFeatures[user][f] + pow(numMoviesRated, -0.5) * numMoviesRated * y[user][f]);
}

// calculates the dot product of the feature vectors for a given user and movie
double predictRating_training(int movie, int user, int numFeatures, int currFeature) {
	assert(user < NUM_USERS);
	assert(user >= 0);
	assert(movie >= 0);
	assert(movie < NUM_MOVIES);

    // at this point the features up to currFeature - 1 are trained and 
    // will not change for the current run of trainSVD()

    double result = 0;

    if (currFeature == 1)
        sumUpToFeatureK[currFeature - 1] = calculateProduct(user, movie, 0);
    else if (currFeature > 1)
        sumUpToFeatureK[currFeature - 1] = sumUpToFeatureK[currFeature - 2] + calculateProduct(user, movie, currFeature - 1);

    // use the cached sum for features 0 to currFeature - 1
    if (currFeature > 0)
        result = sumUpToFeatureK[currFeature - 1];

    // calculate the dot product for the features that havent been trained yet in this round
    for (int f = currFeature; f < numFeatures; f++)
        result += calculateProduct(user, movie, f);

    //return GLOBAL_AVG_MOVIE_RATING + userBias[user] + movieBias[movie] + result;
    return GLOBAL_AVG_MOVIE_RATING + avgUserOffset[user] + avgMovieOffset[movie] + result;
}

// calculates the dot product of the feature vectors for a given user and movie
double predictRating(int movie, int user, int numFeatures) {
    double result = 0;

    // find the dot product
    for (int f = 0; f < numFeatures; f++)
        result += calculateProduct(user, movie, f);

    //return GLOBAL_AVG_MOVIE_RATING + userBias[user] + movieBias[movie] + result;
    return GLOBAL_AVG_MOVIE_RATING + avgUserOffset[user] + avgMovieOffset[movie] + result;
}

// trains our algorithm given an input data point
void trainSVD(int user, int movie, int rating, int Kindex, int numFeatures) {      
    // iterate over features 0 through N - 1
    for (int k = 0; k < numFeatures; k++) {
        double error = rating - predictRating_training(movie - 1, user - 1, numFeatures, k);
        //double error = rating - predictRating(movie - 1, user - 1, numFeatures);

        double oldUserFeature = userFeatures[user - 1][k];
        double oldMovieFeature = movieFeatures[movie - 1][k];

        // update user feature vector
        userFeatures[user - 1][k] += LEARNING_RATE * (error * oldMovieFeature - K[Kindex] * oldUserFeature);

        // update movie feature vector
        //int numMovieRatings = movieRatingsByUser[user - 1];

        //if (numMovieRatings == 0) {
            movieFeatures[movie - 1][k] += LEARNING_RATE * (error * oldUserFeature - K[Kindex] * oldMovieFeature);
        /*} else {
            double pow1 = pow(numMovieRatings, -0.5);
            movieFeatures[movie - 1][k] += LEARNING_RATE * (error * (oldUserFeature + pow1 * numMovieRatings * y[user - 1][k]) - K[Kindex] * oldMovieFeature);
            y[user - 1][k] += LEARNING_RATE * (error * pow1 * oldMovieFeature - K[Kindex] * y[user - 1][k]);
        }*/

    } // end for loop over features
}

// compute the in-sample error
double computeInSample(int numFeatures) {
    double error = 0;

    // go through each training point and calculate the error for it
    for (int i = 0; i < trainingData->size(); i++) {
        int currUser = trainingData->at(i)->user;
        int currMovie = trainingData->at(i)->movie;
        int actualRating = trainingData->at(i)->rating;
        double predicted = predictRating(currMovie - 1, currUser - 1, numFeatures);

        error += (predicted - actualRating) * (predicted - actualRating);
    }

    return sqrt(error / trainingData->size());
}

// compute the out of sample error
double computeOutOfSample(int numFeatures) {
    double error = 0;
    int numPoints = 0;

    string line;
    ifstream outOfSample(outOfSampleFile, ios::in);

    // go through the input data line by line
    while (getline(outOfSample, line)) {
        // where are we in the current line?
        int count = 0;

        int user = -1;
        int movie = -1;
        int actualRating = -1;

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

        double predictedRating = predictRating(movie - 1, user - 1, numFeatures);

        // make sure the rating is between 1 and 5
        if (predictedRating > 5)
            predictedRating = 5;
        else if (predictedRating < 1)
            predictedRating = 1;

        // get the squared error for this point
        error += (predictedRating - actualRating) * (predictedRating - actualRating);

        numPoints++;
    }    
    outOfSample.close();

    return sqrt(error / numPoints);
}

// iterate through the epochs and all the data points for each epoch
void learn() {
    stringstream sstm1;
    vector<string> results;

    double Eout = 10^4;
    double prevEout = 10^9;

    // calculate in-sample and out-of-sample errors for the different numbers of features
    
    for (int f = 0; f < LENGTH_NUM_FEATURES; f++) { // iterate over different # of features

        for (int h = 0; h < LENGTH_K; h++) { // iterate over different K's

            cout << "About to call initialize()" << endl;
            initialize(NUM_FEATURES[f]);
            cout << "Done with initialize()" << endl;

            int i = 0;

            // go N times through the data
            for (int i = 0; i < NUM_EPOCHS; i++) {
            //while (i < NUM_EPOCHS && (prevEout - Eout) > EPSILON) {
                //cout << "Current epoch" << "(feature " << f + 1 << ", K-value " << h + 1 << ") : " << i + 1 << " out of " << NUM_EPOCHS << endl;

                const clock_t begin_time = clock();

                // go through each point in the data set
                for (int p = 0; p < trainingData->size(); p++) {
                    if (p % 1000000 == 0)
                        cout << "Data point: " << p / 1000000 << " million" << endl;

                    // train the SVD on each point (will go through all features)
                    trainSVD(trainingData->at(p)->user, trainingData->at(p)->movie, 
                        trainingData->at(p)->rating, K[h], NUM_FEATURES[f]);

                } // end training data loop

                // decrease the learning rate
                LEARNING_RATE *= LEARNING_RATE_DECREASE_RATE;

                //prevEout = Eout;
                //Eout = computeOutOfSample(NUM_FEATURES[f]);

                cout << "Finished epoch: " << i + 1 << " out of " << NUM_EPOCHS << ", Eout = " << computeOutOfSample(NUM_FEATURES[f]) << endl;

                cout << " --> " << float(clock() - begin_time) / CLOCKS_PER_SEC << " seconds" << endl;

                //i++;

            } // end epoch loop

            // save the results

            sstm1.str("");
            sstm1 << K[h];
            string Kstr = sstm1.str();
            sstm1.str("");

            sstm1 << NUM_FEATURES[f];
            string numF = sstm1.str();
            sstm1.str("");

            sstm1 << computeInSample(NUM_FEATURES[f]);
            string in = sstm1.str();
            sstm1.str("");

            sstm1 << computeOutOfSample(NUM_FEATURES[f]);
            string out = sstm1.str();

            string currResult(Kstr + string(", ") + numF + string(", ") + in + string(", ") + out);

            results.push_back(currResult);

        } // end K loop

    } // end feature loop

    // print out the results
    cout << endl << "RESULTS: (K, # of features, Ein, Eout)" << endl;
    for (int i = 0; i < results.size(); i++)
        cout << results[i] << endl;
    cout << endl;
}

// computes the average ratings for each user (uses UM data set)
// also computes the user offset
void getAvgUserRatings() {
    string line;

    // get the UM data file
    ifstream dataFile(dataFilePathUM, ios::in);
    
    int numLinesSoFar = 0;

    int prevUser = 1;
    double totalRating = 0;
    int index = 0; // counts the # of movies each user rated

    double currTotalUserOffset = 0;

    // go through the input data line by line
    while (getline(dataFile, line)) {
        // where are we in the current line?
        int count = 0;

        int currUser = -1;
        int currMovie = -1;
        int currRating = -1;

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
            avgUserRatings[prevUser - 1] = totalRating / index;
            totalRating = currRating;

            avgUserOffset[prevUser - 1] = currTotalUserOffset / index;
            currTotalUserOffset = currRating - avgMovieRatings[currMovie - 1];
            movieRatingsByUser[prevUser - 1] = index;

            index = 1;
        } else { // if we keep seeing data for the same user
            if (currRating != 0) {
                index++;
                totalRating += currRating;
                currTotalUserOffset += (currRating - avgMovieRatings[currMovie - 1]);
            }
        }

        prevUser = currUser;
            
        numLinesSoFar++;
        if (numLinesSoFar % 1000000 == 0)
            cout << "Computing avg user ratings: " << numLinesSoFar / 1000000 << " million" << endl;
    }    

    // deals with end of file
    avgUserRatings[prevUser - 1] = totalRating / index;
    avgUserOffset[prevUser - 1] = currTotalUserOffset / index;
    movieRatingsByUser[prevUser - 1] = index;

    dataFile.close();
}

// reads data in (MU) and also computes avg ratings for each movie
// also computes the movie offset
void getData() {
    string line;
    ifstream dataFile;
    
    if (MAC)
    	dataFile.open(dataFilePathMac, ios::in);
    else
    	dataFile.open(dataFilePath, ios::in);
    
    int numLinesSoFar = 0;

    int prevMovie = 1;
    double totalRating = 0;
    int index = 0; // counts the # of users that watched each movie

    double currTotalMovieOffset = 0;

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
            avgMovieRatings[prevMovie - 1] = totalRating / index;
            totalRating = currPoint->rating;

            avgMovieOffset[prevMovie - 1] = currTotalMovieOffset / index;
            currTotalMovieOffset = currPoint->rating - avgUserRatings[currPoint->user - 1];

            index = 1;
        } else {
            if (currPoint->rating != 0) {
                index++;
                totalRating += currPoint->rating;
                currTotalMovieOffset += (currPoint->rating - avgUserRatings[currPoint->user - 1]);
            }
        }

        prevMovie = currPoint->movie;
            
        numLinesSoFar++;
        if (numLinesSoFar % 1000000 == 0)
            cout << numLinesSoFar / 1000000 << " million" << endl;
    }    

    // deals with last line with respect to calculating the average movie rating
    avgMovieRatings[prevMovie - 1] = totalRating / index;
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
    	outputFile.open(outputFilePathMac, ios::out);
    else
    	outputFile.open(outputFilePath, ios::out);
    
    // qual file
    string line;
    ifstream qualFile;
    if (MAC)
    	qualFile.open(qualFilePathMac, ios::in);
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
        double predictedScore = predictRating(movie - 1, user - 1, NUM_FEATURES[0]);

        // make sure the rating is between 1 and 5
        if (predictedScore > 5)
            predictedScore = 5;
        else if (predictedScore < 1)
            predictedScore = 1;

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

    for (int i = 0; i < trainingData->size(); i++)
        delete trainingData->at(i);
    delete trainingData;

    delete[] avgMovieRatings;
    delete[] avgUserRatings;

    delete[] avgUserOffset;
    delete[] avgMovieOffset;

    delete[] sumUpToFeatureK;

    //delete[] userBias;
    //delete[] movieBias;

    delete[] movieRatingsByUser;

    for (int i = 0; i < NUM_USERS; i++)
        delete[] y[i];
    delete[] y;
}

// K-value, # features, # epochs
int main(int argc, char *argv[]) {
    // process the command line args
    if (argc != 4) {
        cout << "Usage: ./SVDadv [K-value] [# features] [# epochs]" << endl;
        return 1;
    }

    // process the command-line args
    K[0] = atof(argv[1]);
    NUM_FEATURES[0] = atoi(argv[2]);
    NUM_EPOCHS = atoi(argv[3]);

    getData();
    cout << "Done with getData()" << endl;
    
    // run the SVD algorithm
    learn();
    cout << "Done learning" << endl;

    outputResults();
    cout << "Done with outputResults()" << endl;

    cleanup();
    cout << "DONE!" << endl;
    
    return 0;
}