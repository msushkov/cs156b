#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cassert>

#include <stdio.h>
#include <stdlib.h>
#include <cmath>

using namespace std;

const int MAC = 0;

// Filenames
const char dataFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data1.txt";
const char qualFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVDstart.txt";

const char dataFilePathMac[] = "/users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data2.txt";
const char qualFilePathMac[] = "/users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePathMac[] = "/users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVDstart.txt";

// constants
const int NUM_FEATURES = 15;
const int NUM_USERS = 458293;
const int NUM_MOVIES = 17770;
const double LEARNING_RATE = 0.001;
const int K = 0.02;
const int EPSILON = 10^-5;
const int MAX_ITERATIONS = 1;
const int NUM_EPOCHS = 80;

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
vector<DataPoint*> *trainingData;

// feature matrices (2D arrays)
// feature k for user u is userFeatures[u][k]
double **userFeatures = new double*[NUM_USERS];
double **movieFeatures = new double*[NUM_MOVIES];

// initialize each element of the feature matrices to 0.1
void initialize() {
    // initialize the user feature matrix to all 0.1's
    for (int i = 0; i < NUM_USERS; i++)
        userFeatures[i] = new double[NUM_FEATURES];
    for (int i = 0; i < NUM_USERS; i++) {
        for (int j = 0; j < NUM_FEATURES; j++)
            userFeatures[i][j] = 0.1;
    }
        
    // initialize the movie feature matrix to all 0.1's
    for (int i = 0; i < NUM_MOVIES; i++)
        movieFeatures[i] = new double[NUM_FEATURES];
    for (int i = 0; i < NUM_MOVIES; i++) {
        for (int j = 0; j < NUM_FEATURES; j++)
            movieFeatures[i][j] = 0.1;
    }

    trainingData = new vector<DataPoint*>;
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
double predictRating(int movie, int user) {
	assert(user < NUM_USERS);
	assert(user >= 0);
	assert(movie >= 0);
	assert(movie < NUM_MOVIES);

    double result = 0.0;
    for (int i = 0; i < NUM_FEATURES; i++)
        result += (userFeatures[user][i]) * (movieFeatures[movie][i]);
    return result;
}

// trains our algorithm given an input data point
void trainSVD(int user, int movie, int rating) {
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
        for (int k = 0; k < NUM_FEATURES; k++) {
            double error = rating - predictRating(movie - 1, user - 1);

            double oldUserFeature = userFeatures[user - 1][k];
            userFeatures[user - 1][k] += LEARNING_RATE * (error * movieFeatures[movie - 1][k] - K * oldUserFeature);
            movieFeatures[movie - 1][k] += LEARNING_RATE * (error * oldUserFeature - K * movieFeatures[movie - 1][k]);
        }
            
        numIterations++;
    }
}

// iterate through the epochs and all the data points for each epoch
void learn() {
    // go N times through the data
    for (int i = 0; i < NUM_EPOCHS; i++) {
        cout << "Current epoch: " << i + 1 << " out of " << NUM_EPOCHS << endl;

        // go through each point in the data set
        for (int p = 0; p < trainingData->size(); p++) {
            // train the SVD on each point (will go through all features)
            trainSVD(trainingData->at(p)->user, trainingData->at(p)->movie, trainingData->at(p)->rating);
        }
    }
}

// reads data in
void getData() {
    string line;
    ifstream dataFile;
    
    if (MAC)
    	dataFile.open(dataFilePathMac, ios::in);
    else
    	dataFile.open(dataFilePath, ios::in);
    
    int numLinesSoFar = 0;

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
            
        numLinesSoFar++;
        if (numLinesSoFar % 1000000 == 0)
            cout << numLinesSoFar / 1000000 << " million" << endl;
    }    
    dataFile.close();
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
        double predictedScore = predictRating(movie - 1, user - 1);

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
}

int main() {
    initialize();

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