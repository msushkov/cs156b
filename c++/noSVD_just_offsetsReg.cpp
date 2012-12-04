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
const char dataFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123_86%of4.txt";
const char dataFilePathUM[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123_86%of4.txt";
const char outOfSampleFile[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data_7%of4(valid).txt";
const char qualFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char aggrFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data_7%of4(aggr).txt";
const char aggrOutFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/baselines/regularizedBaseline_aggr.txt";
const char outputFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/no_learning/regularizedBaseline_qual.txt";

const char dataFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123.txt";
const char dataFilePathUMMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123.txt";
const char outOfSampleFileMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data4.txt";
const char qualFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/SVD_40f_min150e_K=0.02.txt";

// constants
const float GLOBAL_AVG_MOVIE_RATING = 3.60951619727;
const int NUM_USERS = 458293;
const int NUM_MOVIES = 17770;

const float EPSILON = 0.0001;
const float VARIANCE_RATIO = 25;

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

// arrays to hold the average ratings for each movie and for each user
float *avgUserRatings = new float[NUM_USERS];
float *avgMovieRatings = new float[NUM_MOVIES];

// the average offset for a given user
float *userBias = new float[NUM_USERS];
float *movieBias = new float[NUM_MOVIES];

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

double predictRating(int movie, int user) {
    return GLOBAL_AVG_MOVIE_RATING + movieBias[movie] + userBias[user];
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

        // if the user changes
        if (prevUser != currUser) {
            avgUserRatings[prevUser - 1] = totalRating / index;
            //avgUserRatings[prevUser - 1] = (GLOBAL_AVG_MOVIE_RATING * VARIANCE_RATIO + totalRating) / (VARIANCE_RATIO + index);
            totalRating = currRating;

            userBias[prevUser - 1] = (currTotalUserOffset) / (index + 10.0);

            currTotalUserOffset = currRating - GLOBAL_AVG_MOVIE_RATING - movieBias[currMovie - 1];

            index = 1;

        } else { // if we keep seeing data for the same user
            index++;
            totalRating += currRating;
            currTotalUserOffset += (currRating - GLOBAL_AVG_MOVIE_RATING - movieBias[currMovie - 1]);
        }

        prevUser = currUser;
            
        numLinesSoFar++;
        if (numLinesSoFar % 10000000 == 0)
            cout << "Computing avg user ratings: " << numLinesSoFar / 1000000 << " million" << endl;
    }    

    // deals with end of file
    avgUserRatings[prevUser - 1] = totalRating / index;
    userBias[prevUser - 1] = currTotalUserOffset / index;

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
            avgMovieRatings[prevMovie - 1] = totalRating / index;
            //avgMovieRatings[prevMovie - 1] = (VARIANCE_RATIO * GLOBAL_AVG_MOVIE_RATING + totalRating) / (VARIANCE_RATIO + index);
            
            totalRating = currPoint->rating;

            movieBias[prevMovie - 1] = (currTotalMovieOffset) / (index + 25.0);

            currTotalMovieOffset = currPoint->rating - GLOBAL_AVG_MOVIE_RATING;

            index = 1;
        } else {
            index++;
            totalRating += currPoint->rating;

            currTotalMovieOffset += (currPoint->rating - GLOBAL_AVG_MOVIE_RATING);
        }

        prevMovie = currPoint->movie;
            
        numLinesSoFar++;
        if (numLinesSoFar % 10000000 == 0)
            cout << numLinesSoFar / 1000000 << " million" << endl;
    }    

    // deals with last line with respect to calculating the average movie rating
    //avgMovieRatings[prevMovie - 1] = totalRating / index;
    avgMovieRatings[prevMovie - 1] = (VARIANCE_RATIO * GLOBAL_AVG_MOVIE_RATING + totalRating) / (VARIANCE_RATIO + index);
    
    movieBias[prevMovie - 1] = currTotalMovieOffset / index;

    dataFile.close();

    // at this point we have the avg movie ratings filled out
    // now calculate the avg user ratings
    getAvgUserRatings();
}

void outputAggr() {
    // output file
    ofstream outputFile;
    outputFile.open(aggrOutFilePath, ios::out);
    
    // aggregation file
    string line;
    ifstream aggrFile;
    aggrFile.open(aggrFilePath, ios::in);

    // go through each line of the qual file
    while (getline(aggrFile, line)) {
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

        // make sure the rating is between 1 and 5
        if (predictedScore > 5)
            predictedScore = 5;
        else if (predictedScore < 1)
            predictedScore = 1;

        // write to file
        outputFile << predictedScore << endl;
    }

    aggrFile.close();
    outputFile.close();
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
        double predictedScore = predictRating(movie - 1, user - 1);

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

    outputAggr();
}

// K-value, # features, # epochs
int main(int argc, char *argv[]) {
    getData();
    cout << "Done with getData()" << endl;

    outputResults();
    cout << "Done with outputResults()" << endl;

    cout << "DONE!" << endl;
    
    return 0;
}