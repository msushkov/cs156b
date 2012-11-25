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
const int MAC = 1;

// Filenames
const char dataFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123.txt";
const char dataFilePathUM[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123.txt";
const char outOfSampleFile[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data4.txt";
const char qualFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/SVDfast_40f_40e_K=0.02.txt";

const char outputUserFeatureFile[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/feature_matrices/userFeatures_SVD_40f_K=0.02_240e.txt";
const char outputMovieFeatureFile[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/feature_matrices/movieFeatures_SVD_40f_K=0.02_240e.txt";

const char dataFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123.txt";
const char dataFilePathUMMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123.txt";
const char outOfSampleFileMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data4.txt";
const char qualFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/SVDreg_40f_min150e_K=0.02.txt";

const char outputUserFeatureFileMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/feature_matrices/userFeatures_SVD_40f_K=0.02_min150e.txt";
const char outputMovieFeatureFileMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/feature_matrices/movieFeatures_SVD_40f_K=0.02_min150e.txt";

// constants
const double GLOBAL_AVG_MOVIE_RATING = 3.60951619727;
const int NUM_USERS = 458293;
const int NUM_MOVIES = 17770;
double LEARNING_RATE_USER = 0.001;
double LEARNING_RATE_MOVIE = 0.001;
const double EPSILON = 0.0001;
const int LENGTH_NUM_FEATURES = 1;
const int LENGTH_K = 1;
const double LEARNING_RATE_DECREASE_RATE = 1;
const double VARIANCE_RATIO = 25;
const double LRATE_ub = 0.012;
const double LRATE_mb = 0.003;
const double LAMBDA_ub = 0.03;
const double LAMBDA_mb = 0.01;

// to be adjusted (and set by command line args)
int NUM_FEATURES[LENGTH_NUM_FEATURES];
double K[LENGTH_K];
int NUM_EPOCHS;

// defines a single training data point
class DataPoint {
public:
    int user;
    int movie;
    double rating;

    DataPoint() {
        this->user = 0;
        this->movie = 0;
        this->rating = 0;
    }

    DataPoint(unsigned int user, unsigned int movie, double rating) {
        this->user = user;
        this->movie = movie;
        this->rating = rating;
    }
};

// helps us to get a "better mean" for a given user or movie rating
class SumAndNumberRatings {
public:
    double sumRatings;
    double numRatings;

    SumAndNumberRatings(double sumRatings, double numRatings) {
        this->sumRatings = sumRatings;
        this->numRatings = numRatings;
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

// hold the sum and count of ratings for each user or movie
SumAndNumberRatings **movieSumsAndCounts = new SumAndNumberRatings*[NUM_MOVIES];
SumAndNumberRatings **userSumsAndCounts = new SumAndNumberRatings*[NUM_USERS];

// the regularized movie averages
double *betterMovieMeans = new double[NUM_MOVIES];

// the average offset for a given user (regularized version)
double *betterUserMeans = new double[NUM_USERS];

// b_u and b_i
double *userBias = new double[NUM_USERS];
double *movieBias = new double[NUM_MOVIES];

// For each user, the number of movies they rated
int *movieRatingsByUser = new int[NUM_USERS];

// the inverse square root of the number of movie ratings
double *norm = new double[NUM_USERS];

// stores the y-values of each y[user][feature]
//double **y = new double*[NUM_USERS];

void computeNorms() {
    for (int i = 0; i < NUM_USERS; i++) {
        int numMovies = movieRatingsByUser[i];

        if (numMovies > 0)
            norm[i] = pow(numMovies, -0.5);
    }
}

// returns a random value between -0.01 and 0.01
double getRandom() {
	//return 10.0 / 10.0 * ((double) rand() / ((double) RAND_MAX)) - 0.0;
	int num = ((int) rand()) % 100;
	return 2.0 * num / 10000.0 - 0.01;
}

// initialize each element of the feature matrices
void initialize(int numFeatures) {
    // initialize the user feature matrix to all 0.1's
    for (int i = 0; i < NUM_USERS; i++)
        userFeatures[i] = new double[numFeatures];
    for (int i = 0; i < NUM_USERS; i++) {
        for (int j = 0; j < numFeatures; j++) {
            //userFeatures[i][j] = 0.1;
            double rand = sqrt(GLOBAL_AVG_MOVIE_RATING / (double) NUM_FEATURES[0]) + getRandom();
            //cout << rand << endl;
            userFeatures[i][j] = rand;
        }
    }
        
    // initialize the movie feature matrix to all 0.1's
    for (int i = 0; i < NUM_MOVIES; i++)
        movieFeatures[i] = new double[numFeatures];
    for (int i = 0; i < NUM_MOVIES; i++) {
        for (int j = 0; j < numFeatures; j++) {
            //movieFeatures[i][j] = 0.1;
            double rand = sqrt(GLOBAL_AVG_MOVIE_RATING / (double) NUM_FEATURES[0]) + getRandom();
            //cout << rand << endl;
            movieFeatures[i][j] = rand;
        }
    }

    // initialize y
    /*for (int i = 0; i < NUM_USERS; i++)
        y[i] = new double[numFeatures];*/

    computeNorms();
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

void computeBetterMeans() {
    for (int i = 0; i < NUM_MOVIES; i++) {
        SumAndNumberRatings *s = movieSumsAndCounts[i];

        double sumForCurrMovie = 0;
        double numRatingsForCurrMovie = 0;

        if (s != NULL) {
            sumForCurrMovie = s->sumRatings;
            numRatingsForCurrMovie = s->numRatings;
        }
        
        betterMovieMeans[i] = (VARIANCE_RATIO * GLOBAL_AVG_MOVIE_RATING + sumForCurrMovie) / (VARIANCE_RATIO + numRatingsForCurrMovie);
    }

    for (int i = 0; i < NUM_USERS; i++) {
        SumAndNumberRatings *s = userSumsAndCounts[i];

        double sumForCurrUser = 0;
        double numRatingsForCurrUser = 0;

        if (s != NULL) {
            sumForCurrUser = s->sumRatings;
            numRatingsForCurrUser = s->numRatings;
        }
        
        betterUserMeans[i] = (VARIANCE_RATIO * GLOBAL_AVG_MOVIE_RATING + sumForCurrUser) / (VARIANCE_RATIO + numRatingsForCurrUser);
    }
}

// calculates a single product for a given user, movie, and feature
inline double calculateProduct(int user, int movie, int f) {
    return movieFeatures[movie][f] * userFeatures[user][f];
    //return movieFeatures[movie][f] * (userFeatures[user][f] + norm[user] * numMoviesRated * y[user][f]);
}

// calculates the dot product of the feature vectors for a given user and movie
/*double predictRating_training(int movie, int user, int numFeatures, int currFeature) {
    // at this point the features up to currFeature - 1 are trained and 
    // will not change for the current run of trainSVD()

    double result = 0;

    if (currFeature >= 1) {
        double prod = calculateProduct(user, movie, currFeature - 1);
        sumUpToFeatureK[currFeature - 1] = prod;
        result = prod;

        if (currFeature > 1)
            sumUpToFeatureK[currFeature - 1] += sumUpToFeatureK[currFeature - 2];
    }        

    // calculate the dot product for the features that havent been trained yet in this round
    for (int f = currFeature; f < numFeatures; f++)
        result += calculateProduct(user, movie, f);

    //return GLOBAL_AVG_MOVIE_RATING + userBias[user] + movieBias[movie] + result;
    return GLOBAL_AVG_MOVIE_RATING + avgUserOffset[user] + avgMovieOffset[movie] + result;
    //return GLOBAL_AVG_MOVIE_RATING + (betterUserMeans[user] - GLOBAL_AVG_MOVIE_RATING) + (betterMovieMeans[movie] - GLOBAL_AVG_MOVIE_RATING) + result;
    //return result;
}*/

double predictRating_training(int user, int movie, double *movieFeatureVec, double *userFeatureVec, int numFeatures, int currFeature) {
    // at this point the features up to currFeature - 1 are trained and 
    // will not change for the current run of trainSVD()

    //double result = GLOBAL_AVG_MOVIE_RATING;
    double result = 0;

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

/*
// calculates the dot product of the feature vectors for a given user and movie
double predictRating(int movie, int user, double *userFeature, double *movieFeature, int numFeatures) {
    //double result = GLOBAL_AVG_MOVIE_RATING;
	double result = 0;

    // find the dot product
    for (int f = 0; f < numFeatures; f++)
        result += (userFeature[f] * movieFeature[f]);

    //eturn avgUserOffset[user] + avgMovieOffset[movie] + userBias[user] + movieBias[movie] + result;
    //return avgUserOffset[user] + avgMovieOffset[movie] + result;
    //return avgUserOffset[user] + avgMovieRatings[movie] + result;
    return result;
}*/

double predictRating(int movie, int user, int numFeatures, double userOffset, double movieRating) {
	//double result = GLOBAL_AVG_MOVIE_RATING;
	//double result = userOffset + movieRating;
	double result = 0;

    // find the dot product
    for (int f = 0; f < numFeatures; f++) {
        result += (userFeatures[user][f] * movieFeatures[movie][f]);
    }

    return result;
}

// trains our algorithm given an input data point
void trainSVD(int user, int movie, double rating, int Kindex, double numFeatures) {
    //double *userFeature = userFeatures[user - 1];
    //double *movieFeature = movieFeatures[movie - 1];
    
    double userOffset = avgUserOffset[user - 1];
	double movieRating = avgMovieRatings[movie - 1];

    //double error = rating - predictRating(movie - 1, user - 1, numFeatures);

    //userBias[user - 1] += (LRATE_ub * (error - LAMBDA_ub * userBias[user - 1]));
    //movieBias[movie - 1] += (LRATE_mb * (error - LAMBDA_mb * movieBias[movie - 1]));

    // iterate over features 0 through N - 1
    for (int k = 0; k < numFeatures; k++) {
        //double error = rating - predictRating_training(user - 1, movie - 1, movieFeature, userFeature, numFeatures, k);
        
        // calculate the error every 5 features
        //if (k % 5 == 0)
            //error = rating - predictRating(movie - 1, user - 1, numFeatures);
        //double error = rating - (predictRating(movie - 1, user - 1, userFeature, movieFeature, numFeatures) + userOffset + movieRating);
        double error = rating - predictRating(movie - 1, user - 1, numFeatures, userOffset, movieRating);

        //double oldUserFeature = userFeature[k];
        //double oldMovieFeature = movieFeature[k];

        double oldUserFeature = userFeatures[user - 1][k];
        double oldMovieFeature = movieFeatures[movie - 1][k];

        // update user feature vector
        userFeatures[user - 1][k] += (LEARNING_RATE_USER * (error * oldMovieFeature - K[Kindex] * oldUserFeature));
        //userFeature[k] += (LEARNING_RATE_USER * (error * oldMovieFeature - K[Kindex] * oldUserFeature));

        // update movie feature vector
        movieFeatures[movie - 1][k] += (LEARNING_RATE_MOVIE * (error * oldUserFeature - K[Kindex] * oldMovieFeature));
        //movieFeature[k] += (LEARNING_RATE_MOVIE * (error * oldUserFeature - K[Kindex] * oldMovieFeature));

        //movieFeatures[movie - 1][k] += LEARNING_RATE * (error * (oldUserFeature + norm[user - 1] * numMovieRatings * y[user - 1][k]) - K[Kindex] * oldMovieFeature);
        //y[user - 1][k] += LEARNING_RATE * (error * norm[user - 1] * oldMovieFeature - K[Kindex] * y[user - 1][k]);

    } // end for loop over features
}

// compute the in-sample error
double computeInSample(int numFeatures) {
    double error = 0;

    // go through each training point and calculate the error for it
    for (int i = 0; i < trainingData->size(); i++) {
        int currUser = trainingData->at(i)->user;
        int currMovie = trainingData->at(i)->movie;
        double actualRating = trainingData->at(i)->rating;
        //double predicted = predictRating(currMovie - 1, currUser - 1, numFeatures);
        double predicted = predictRating(currMovie - 1, currUser - 1, numFeatures, avgUserOffset[currUser - 1], avgMovieRatings[currMovie - 1]);

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
double computeOutOfSample(int numFeatures) {
    double error = 0;
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
        double actualRating = -1;

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

        //double predictedRating = predictRating(movie - 1, user - 1, numFeatures);
        double predictedRating = predictRating(movie - 1, user - 1, numFeatures, avgUserOffset[user - 1], avgMovieRatings[movie - 1]);

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
    // clean some stuff up
    for (int i = 0; i < NUM_USERS; i++) 
        delete userSumsAndCounts[i];

    for (int i = 0; i < NUM_MOVIES; i++) 
        delete movieSumsAndCounts[i];

    delete[] movieSumsAndCounts;
    delete[] userSumsAndCounts;

    stringstream sstm1;
    vector<string> results;

    double Eout = 10000;
    double prevEout = 1000000;

    // calculate in-sample and out-of-sample errors for the different numbers of features
    
    for (int f = 0; f < LENGTH_NUM_FEATURES; f++) { // iterate over different # of features

        for (int h = 0; h < LENGTH_K; h++) { // iterate over different K's
            initialize(NUM_FEATURES[f]);

            int i = 0;

            // go N times through the data
            //for (int i = 0; i < NUM_EPOCHS; i++) {
            while (i < NUM_EPOCHS || (prevEout - Eout) > EPSILON) {
                //cout << "Current epoch" << "(feature " << f + 1 << ", K-value " << h + 1 << ") : " << i + 1 << " out of " << NUM_EPOCHS << endl;

                const clock_t begin_time = clock();

                // go through each point in the data set
                for (int p = 0; p < trainingData->size(); p++) {
                    if (p % 30000000 == 0)
                        cout << "Data point: " << p / 1000000 << " million" << endl;

                    // train the SVD on each point (will go through all features)
                    trainSVD(trainingData->at(p)->user, trainingData->at(p)->movie, 
                        trainingData->at(p)->rating, K[h], NUM_FEATURES[f]);

                } // end training data loop

                // randomize the training data
                next_permutation(trainingData->begin(), trainingData->end());

                // decrease the learning rates
                LEARNING_RATE_USER *= LEARNING_RATE_DECREASE_RATE;
                LEARNING_RATE_MOVIE *= LEARNING_RATE_DECREASE_RATE;

                // only check Eout every 5 iterations
                if (i % 3 == 0) {
                    prevEout = Eout;
                    Eout = computeOutOfSample(NUM_FEATURES[f]);
                    cout << "    Eout = " << Eout << endl;
                }

                cout << "Finished epoch: " << i + 1 << " out of " << NUM_EPOCHS << endl;
                cout << " --> " << double(clock() - begin_time) / CLOCKS_PER_SEC << " seconds" << endl;

                i++;

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
    ifstream dataFile;
    if (MAC == 0)
    	dataFile.open(dataFilePathUM, ios::in);
    else
    	dataFile.open(dataFilePathUMMAC, ios::in);
    
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
        double currRating = -1;

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
            userSumsAndCounts[prevUser - 1] = new SumAndNumberRatings(totalRating, index);

            //avgUserRatings[prevUser - 1] = totalRating / index;
            avgUserRatings[prevUser - 1] = (GLOBAL_AVG_MOVIE_RATING * VARIANCE_RATIO + totalRating) / (VARIANCE_RATIO + index);
            totalRating = currRating;

            avgUserOffset[prevUser - 1] = currTotalUserOffset / index;
            currTotalUserOffset = currRating - avgMovieRatings[currMovie - 1];
            //currTotalUserOffset = currRating - GLOBAL_AVG_MOVIE_RATING;
            movieRatingsByUser[prevUser - 1] = index;

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
    movieRatingsByUser[prevUser - 1] = index;
    userSumsAndCounts[prevUser - 1] = new SumAndNumberRatings(totalRating, index);

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
            movieSumsAndCounts[prevMovie - 1] = new SumAndNumberRatings(totalRating, index);

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
    movieSumsAndCounts[prevMovie - 1] = new SumAndNumberRatings(totalRating, index);

    dataFile.close();

    // at this point we have the avg movie ratings filled out
    // now calculate the avg user ratings
    getAvgUserRatings();

    //computeBetterMeans();
}

void outputVectors() {
    cout << "Output vectors" << endl;

    // output files
    ofstream outputFileUser;
    
    if (MAC == 0)
	    outputFileUser.open(outputUserFeatureFile, ios::out);
	else
		outputFileUser.open(outputUserFeatureFileMAC, ios::out);
	
    ofstream outputFileMovie;
    if (MAC == 0)
	    outputFileMovie.open(outputMovieFeatureFile, ios::out);
    else
    	outputFileMovie.open(outputMovieFeatureFileMAC, ios::out);
    
    // output the user features
    for (int i = 0; i < NUM_USERS; i++) {
        for (int k = 0; k < NUM_FEATURES[0]; k++) {
            outputFileUser << userFeatures[i][k] << " ";
        }

        outputFileUser << endl;
    }

    // output the movie features
    for (int i = 0; i < NUM_MOVIES; i++) {
        for (int k = 0; k < NUM_FEATURES[0]; k++) {
            outputFileMovie << movieFeatures[i][k] << " ";
        }

        outputFileMovie << endl;
    }

    outputFileMovie.close();
    outputFileUser.close();
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
        double predictedScore = predictRating(movie - 1, user - 1, NUM_FEATURES[0], avgUserOffset[user - 1], avgMovieRatings[movie - 1]);

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

    outputVectors();
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

    //delete[] userBias;
    //delete[] movieBias;

    delete[] movieRatingsByUser;

    delete[] norm;

    /*for (int i = 0; i < NUM_USERS; i++)
        delete[] y[i];
    delete[] y;*/

    delete[] betterMovieMeans;
    delete[] betterUserMeans;
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

    //cleanup();
    cout << "DONE!" << endl;
    
    return 0;
}