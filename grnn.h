#ifndef GRNN_H
#define GRNN_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <random>
#include <ctype.h>
#include <cstring>
#include <stdlib.h>
#include <iomanip>
#include <stack>
#include <stdio.h>
#include <cstdlib>

#define NUM_DATA_POINTS 595
#define NUM_FEATURES 95

using namespace std;

class Data_Point {
    public:
    Data_Point();
    Data_Point(string line);
    Data_Point(string line, int k);
    int pnt_id; // Identification number
    double clsfr; // Classifier
    double feat_vecs[NUM_FEATURES]; // Array for each of the 95 features of each element
    double grnn_classify(Data_Point trng_set[], double* sigma, int pop_index);
    double grnn_classify(Data_Point trng_set[], double sigma);
    double grnn_classify(Data_Point trng_set[], double sigma, double weights[]);
    double knn_classify(Data_Point trng_set[], int k, double b);
    };


#endif
