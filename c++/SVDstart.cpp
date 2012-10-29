#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <cmath>

using namespace std;

// Filenames
const char dataFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data2.txt";
const char qualFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVDstart.txt";

// constants
const int NUM_FEATURES = 6;
const int NUM_USERS = 458293;
const int NUM_MOVIES = 17770;
const double LEARNING_RATE = 0.001;
const int K = 25;
const int EPSILON = 10^-5;
const int MAX_ITERATIONS = 20;

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
    double result = 0.0;

    for (int i = 0; i < NUM_FEATURES; i++) {
        cout << userFeatures[user][i] << " :: " << movieFeatures[movie][i] << endl;
        double prod = (userFeatures[user][i]) * (movieFeatures[movie][i]);
        result += prod;
    }

    return result;
}

// trains our algorithm given an input data point
void trainSVD(int user, int movie, int rating) {
    int numIterations = 0;

    // large so we go into the first iteration of the while loop
    double error = 10^9;
    
    // learn while our error is decreasing by an amount higher than epsilon
    // or until we hit some # of iterations
    //while (abs(LEARNING_RATE * (rating - predictRating(movie - 1, user - 1)) - error) > EPSILON && numIterations < MAX_ITERATIONS) {
    while (numIterations < MAX_ITERATIONS) {
        // calculate the new error
        //cout << predictRating(movie - 1, user - 1) << endl;

        error = rating - predictRating(movie - 1, user - 1);
        //cout << "ERROR: " << error << endl;
        //cout << user << "::" << movie << endl;
    
        // iterate over features 0 through 39
        for (int k = 0; k < NUM_FEATURES; k++) {            
            double oldUserFeature = userFeatures[user - 1][k];
            //userFeatures[user - 1][k] += LEARNING_RATE * (error * movieFeatures[movie - 1][k] - K * oldUserFeature);
            //movieFeatures[movie - 1][k] += LEARNING_RATE * (error * oldUserFeature - K * movieFeatures[movie - 1][k]);

            userFeatures[user - 1][k] += LEARNING_RATE * error * movieFeatures[movie - 1][k];
            movieFeatures[movie - 1][k] += LEARNING_RATE * error * oldUserFeature;

            cout << userFeatures[user - 1][k] << endl;
            cout << movieFeatures[movie - 1][k] << endl;
        }
            
        numIterations++;
    }
}

// reads data in and loads it into memory
void getData() {
    string line;
    ifstream dataFile(dataFilePath, ios::in);
    
    int numLinesSoFar = 0;
    
    // go through the input data line by line
    if (dataFile.is_open()) {
        while (dataFile.good() && numLinesSoFar < 10) {
            if (getline(dataFile, line)) {                
                vector<string> vals = split(line, ' ');

                // call the training function using the given data point
                trainSVD(atoi(vals[0].c_str()), atoi(vals[1].c_str()), atoi(vals[3].c_str()));
                
                numLinesSoFar++;
                if (numLinesSoFar % 100000 == 0)
                    cout << numLinesSoFar << endl;
            }
        }
    }
    
    dataFile.close();
}

// writes data out
void outputResults() {
    // output file
    ofstream outputFile(outputFilePath, ios::out);
    
    // qual file
    string line;
    ifstream qualFile(qualFilePath, ios::in);
    
    // go through each line of the qual file
    if (qualFile.is_open()) {
        while (qualFile.good()) {
            if (getline(qualFile, line)) {
                vector<string> vals = split(line, ' ');

                //cout << atoi(vals[0].c_str()) - 1 << " :: " << atoi(vals[1].c_str()) - 1<< endl;

                int user = atoi(vals[0].c_str()) - 1;
                cout << user << endl;
                /*for (int j = 0; j < NUM_FEATURES; j++) {
                    cout << userFeatures[1][j] << ", ";
                }*/

                // calculate the rating for user, movie
                double predictedScore = predictRating(atoi(vals[0].c_str()) - 1, atoi(vals[1].c_str()) - 1);

                // write to file
                if (outputFile.is_open()) {
                    // convert double to string first
                    stringstream s;
                    s << predictedScore;
                    
                    outputFile << s.str() << endl;
                }
            
            }
        }
        
        qualFile.close();
        outputFile.close();
    }
}

void cleanup() {
    for (int i = 0; i < NUM_USERS; i++)
        delete[] userFeatures[i];
        
    for (int i = 0; i < NUM_MOVIES; i++)
        delete[] movieFeatures[i];
        
    delete[] userFeatures;
    delete[] movieFeatures;
}

void debugPrint() {
    for (int i = 0; i < NUM_USERS; i++) {
        for (int j = 0; j < NUM_FEATURES; j++) {
            cout << userFeatures[i][j] << ", ";
        }
        cout << endl;
    }

    for (int i = 0; i < NUM_MOVIES; i++) {
        for (int j = 0; j < NUM_FEATURES; j++) {
            cout << movieFeatures[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    initialize();

    getData();
    cout << "Done with getData()" << endl;
    
    outputResults();
    //debugPrint();
    cout << "Done with outputResults()" << endl;

    cleanup();

    cout << "DONE!" << endl;
    
    return 0;
}