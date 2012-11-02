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
const char outOfSampleFile[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data2.txt";
const char qualFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD_25f_40e_K=0.01.txt";

const char dataFilePathMac[] = "/users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data2.txt";
const char qualFilePathMac[] = "/users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePathMac[] = "/users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVDstart.txt";

// constants
const int NUM_USERS = 458293;
const int NUM_MOVIES = 17770;
const double LEARNING_RATE = 0.001;
const int MAX_ITERATIONS = 1;
const int EPSILON = 10^(-4);

// to be adjusted (and set by command line args)
const int LENGTH_NUM_FEATURES = 1;
int NUM_FEATURES[LENGTH_NUM_FEATURES];

const int LENGTH_K = 1;
double K[LENGTH_K];

int NUM_EPOCHS;
double VARIANCE_RATIO;

double GLOBAL_AVG_MOVIE_RATING = 0;

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

// feature matrices (2D arrays)
// feature k for user u is userFeatures[u][k]
double **userFeatures = new double*[NUM_USERS];
double **movieFeatures = new double*[NUM_MOVIES];

// arrays to hold the average ratings for each movie and for each user
double *avgUserRatings = new double[NUM_USERS];
double *avgMovieRatings = new double[NUM_MOVIES];

// the average offset for a given user
double *avgOffset = new double[NUM_USERS];

// hold the sum and count of ratings for each user or movie
SumAndNumberRatings **movieSumsAndCounts = new SumAndNumberRatings*[NUM_MOVIES];
SumAndNumberRatings **userSumsAndCounts = new SumAndNumberRatings*[NUM_USERS];

// the regularized movie averages
double *betterMovieMeans = new double[NUM_MOVIES];

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

// calculates the dot product of the feature vectors for a given user and movie
double predictRating(int movie, int user, int numFeatures) {
	assert(user < NUM_USERS);
	assert(user >= 0);
	assert(movie >= 0);
	assert(movie < NUM_MOVIES);

    double result = 0.0;
    for (int i = 0; i < numFeatures; i++)
        result += (userFeatures[user][i]) * (movieFeatures[movie][i]);

    return result + betterMovieMeans[movie] + avgOffset[user];
}

void computeBetterMovieMeans() {
    for (int i = 0; i < NUM_MOVIES; i++) {
        SumAndNumberRatings *s = movieSumsAndCounts[i];

        double sumForCurrMovie = 0;
        double numRatingsForCurrMovie = 0;

        if (s != NULL) {
            sumForCurrMovie = s->sumRatings;
            numRatingsForCurrMovie = s->numRatings;
        }
        
        betterMovieMeans[i] = (VARIANCE_RATIO * GLOBAL_AVG_MOVIE_RATING + sumForCurrMovie) / 
            (VARIANCE_RATIO + numRatingsForCurrMovie);
    }
}

// computes the ratio of the variances (the regularization constant for betterMean)
void computeVarianceRatios() {
    // variance of all the avg movie ratings
    double Va = 0;

    double totalDiff = 0;
    // find the variance
    for (int i = 0; i < NUM_MOVIES; i++)
        totalDiff += (avgMovieRatings[i] - GLOBAL_AVG_MOVIE_RATING) *
            (avgMovieRatings[i] - GLOBAL_AVG_MOVIE_RATING);
    Va = totalDiff / NUM_MOVIES;

    // go through the MU file and get the avg variance of indiv movie ratings
    string line;
    ifstream dataFile(dataFilePath, ios::in);

    int prevMovie = 1;
    double currSquaredSum = 0;
    double totalVariance = 0;
    int index = 0;

    int totalCount = 1;

    // go through each line
    while (getline(dataFile, line)) {
        // where are we in the current line?
        int count = 0;

        int rating = 0;
        int movie = 0;

        istringstream lineIn(line);
        while (lineIn) {
            int val = 0;
            if (lineIn >> val) {
                if (count == 1)
                    movie = val;
                else if (count == 3)
                    rating = val;
                count++;
            }
        }

        if (prevMovie != movie) {
            totalCount++;
            totalVariance += currSquaredSum / index;
            index = 1;
            currSquaredSum = (rating - avgMovieRatings[movie - 1]) * (rating - avgMovieRatings[movie - 1]);
        } else {
            if (rating != 0) {
                index++;
                currSquaredSum += (rating - avgMovieRatings[movie - 1]) * (rating - avgMovieRatings[movie - 1]);
            }
        }

        prevMovie = movie;

    }

    dataFile.close();

    // account for the end of file
    totalVariance += currSquaredSum / index;

    double Vb = totalVariance / totalCount;

    VARIANCE_RATIO = Vb / Va;
}

// trains our algorithm given an input data point
void trainSVD(int user, int movie, int rating, int Kindex, int numFeatures) {
	assert(user <= NUM_USERS);
	assert(user > 0);
	assert(movie > 0);
	assert(rating > 0);
	assert(rating <= 5);
	assert(movie <= NUM_MOVIES);

    int numIterations = 0;

    // learn while our error is decreasing by an amount higher than epsilon
    // or until we hit some # of iterations
    while (numIterations < MAX_ITERATIONS) {       
        // iterate over features 0 through 39
        for (int k = 0; k < numFeatures; k++) {
            double error = rating - predictRating(movie - 1, user - 1, numFeatures);

            double oldUserFeature = userFeatures[user - 1][k];
            userFeatures[user - 1][k] += LEARNING_RATE * (error * movieFeatures[movie - 1][k] - K[Kindex] * oldUserFeature);
            movieFeatures[movie - 1][k] += LEARNING_RATE * (error * oldUserFeature - K[Kindex] * movieFeatures[movie - 1][k]);
        }
            
        numIterations++;
    }
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

            initialize(NUM_FEATURES[f]);

            int i = 0;

            // go N times through the data
            for (int i = 0; i < NUM_EPOCHS; i++) {
            //while (i < NUM_EPOCHS && (prevEout - Eout) > EPSILON) {
                cout << "Current epoch" << "(feature " << f + 1 << " out of " << LENGTH_NUM_FEATURES << 
                    ", K-value is " << h + 1 << " out of " << LENGTH_K << ") : " << i + 1 << " out of " << NUM_EPOCHS << endl;

                const clock_t begin_time = clock();

                // go through each point in the data set
                for (int p = 0; p < trainingData->size(); p++) {

                    // train the SVD on each point (will go through all features)
                    trainSVD(trainingData->at(p)->user, trainingData->at(p)->movie, 
                        trainingData->at(p)->rating, K[h], NUM_FEATURES[f]);

                } // end training data loop

                //prevEout = Eout;
                //Eout = computeOutOfSample(NUM_FEATURES[f]);

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
    cout << endl << "RESULTS: (K, # of features, in-sample, out-of-sample)" << endl;
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

    double currTotalOffset = 0;

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
            userSumsAndCounts[prevUser - 1] = new SumAndNumberRatings(totalRating, index);

            avgUserRatings[prevUser - 1] = totalRating / index;
            totalRating = currRating;

            avgOffset[prevUser - 1] = currTotalOffset / index;
            currTotalOffset = currRating - avgMovieRatings[currMovie - 1];

            index = 1;
        } else { // if we keep seeing data for the same user
            if (currRating != 0) {
                index++;
                totalRating += currRating;
                currTotalOffset += (currRating - avgMovieRatings[currMovie - 1]);
            }
        }

        prevUser = currUser;
            
        numLinesSoFar++;
        if (numLinesSoFar % 1000000 == 0)
            cout << "Computing avg user ratings: " << numLinesSoFar / 1000000 << " million" << endl;
    }    

    // deals with last line with respect to calculating the average user rating
    avgUserRatings[prevUser - 1] = totalRating / index;
    userSumsAndCounts[prevUser - 1] = new SumAndNumberRatings(totalRating, index);
    avgOffset[prevUser - 1] = currTotalOffset / index;

    dataFile.close();
}

// sets the global average of the average ratings for each movie
void getGlobalMovieAvg() {
    double sum = 0;
    int count = 0; // counts the movies that were rated in this dataset
    for (int i = 0; i < NUM_MOVIES; i++) {
        sum += avgMovieRatings[i];

        if (avgMovieRatings[i] > 0)
            count++;
    }

    GLOBAL_AVG_MOVIE_RATING = sum / count;
}

// reads data in (MU) and also cimputes avg ratings for each movie
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

            avgMovieRatings[prevMovie - 1] = totalRating / index;
            index = 1;
            totalRating = currPoint->rating;
        } else {
            if (currPoint->rating != 0)
                index++;
            totalRating += currPoint->rating;
        }

        prevMovie = currPoint->movie;
            
        numLinesSoFar++;
        if (numLinesSoFar % 1000000 == 0)
            cout << numLinesSoFar / 1000000 << " million" << endl;
    }    

    // deals with last line with respect to calculating the average movie rating
    avgMovieRatings[prevMovie - 1] = totalRating / index;
    movieSumsAndCounts[prevMovie - 1] = new SumAndNumberRatings(totalRating, index);

    dataFile.close();

    // at this point we have the avg movie ratings filled out
    // now calculate the avg user ratings
    getAvgUserRatings();

    // compute the global movie average
    getGlobalMovieAvg();

    // populate the better means array
    computeBetterMovieMeans();

    computeVarianceRatios();
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

    for (int i = 0; i < NUM_USERS; i++) 
        delete userSumsAndCounts[i];

    for (int i = 0; i < NUM_MOVIES; i++) 
        delete movieSumsAndCounts[i];

    delete[] movieSumsAndCounts;
    delete[] userSumsAndCounts;

    delete[] avgOffset;

    delete betterMovieMeans;
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