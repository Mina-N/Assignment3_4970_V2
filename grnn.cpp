#include "grnn.h"


Data_Point::Data_Point() {  // Instantiation
    pnt_id = -1;
}

Data_Point::Data_Point(string line) { // Initialization
    istringstream iss;
    iss.str(line);
    
    iss >> pnt_id;
    iss >> clsfr;
    
    int i;
    for(i = 0; i < NUM_FEATURES; i++)
        iss >> feat_vecs[i];
}

Data_Point::Data_Point(string line, int k) { //Constructor for when classification is unknown
    istringstream iss;
    iss.str(line);
    iss >> pnt_id;
    
    for (int i = 0; i < 95; i++) {
        iss >> feat_vecs[i];
    }
}

double Data_Point::grnn_classify(Data_Point trng_set[], double* sigma, int pop_index) {
    double gaussian = 0;
    double weighted_gaussian = 0;
    for (int i = 0; i < NUM_DATA_POINTS; i++) {
        double distance_squared = 0;
        for (int j = 0; j < NUM_FEATURES; j++) {
            distance_squared += pow((trng_set[i].feat_vecs[j] - this->feat_vecs[j]),2);
        }
        if (distance_squared != 0) {
            gaussian += exp(-distance_squared/(2*pow(sigma[NUM_DATA_POINTS*pop_index + i],2)));
            weighted_gaussian += exp(-distance_squared/(2*pow(sigma[NUM_DATA_POINTS*pop_index + i],2))) * trng_set[i].clsfr;
        }
    }
    return weighted_gaussian / gaussian;
}

double Data_Point::grnn_classify(Data_Point trng_set[], double sigma) {
    double gaussian = 0;
    double weighted_gaussian = 0;
    for (int i = 0; i < NUM_DATA_POINTS; i++) {
        double distance_squared = 0;
        for (int j = 0; j < NUM_FEATURES; j++) {
            distance_squared += pow((trng_set[i].feat_vecs[j] - this->feat_vecs[j]),2);
        }
        if (distance_squared != 0) {
            gaussian += exp(-distance_squared/(2*pow(sigma,2)));
            weighted_gaussian += exp(-distance_squared/(2*pow(sigma,2))) * trng_set[i].clsfr;
        }
    }
    return weighted_gaussian / gaussian;
}

double Data_Point::grnn_classify(Data_Point trng_set[], double sigma, double weights[]) {
    double gaussian = 0;
    double weighted_gaussian = 0;
    for (int i = 0; i < NUM_DATA_POINTS; i++) {
        double distance_squared = 0;
        for (int j = 0; j < NUM_FEATURES; j++) {
            distance_squared += weights[j] * pow((trng_set[i].feat_vecs[j] - this->feat_vecs[j]),2);
        }
        if (distance_squared != 0) {
            gaussian += exp(-distance_squared/(2*pow(sigma,2)));
            weighted_gaussian += exp(-distance_squared/(2*pow(sigma,2))) * trng_set[i].clsfr;
        }
    }
    return weighted_gaussian / gaussian;
}

double Data_Point::knn_classify(Data_Point trng_set[], int k, double b) {
    double* k_clsfr;
    k_clsfr = new double[k];
    double* k_distances;
    k_distances = new double[k];
    double min_distance = 80000;
    int min_k_index;
    double distance;
    for (int i = 0; i < k; i++) {
        min_distance = 80000;
        for (int j = 0; j < NUM_DATA_POINTS; j++) {
            distance = 0;
            for (int l = 0; l < NUM_FEATURES; l++) {
                distance += pow((trng_set[j].feat_vecs[l] - this->feat_vecs[l]), 2);
            }
            //cout << "distance: " << distance << endl;
            bool in_array = false;
            for (int m = 0; m < i; m++) {
                if (fabs(distance - k_distances[m]) < 0.0000001) {
                    in_array = true;
                    break;
                }
            }
            if (distance < min_distance && !in_array && distance > 0.0000001) {
                min_distance = distance;
                min_k_index = j;
            }
            
        }
        //cout << "k_distances[i-1] " << k_distances[i-1] << endl;
        //cout << "min_distance " << min_distance << endl;
        //cout << "fabs(distance - k_distances[i-1]): " << fabs(min_distance - k_distances[i-1]) << endl;
        k_distances[i] = min_distance;
        //cout << min_k_index << endl;
        k_clsfr[i] = trng_set[min_k_index].clsfr;
        //cout << k_distances[i] << endl;
    }
    double prediction = 0;
    double denominator = 0;
    for (int i = 0; i < k; i++) {
        prediction += k_clsfr[i] / pow(k_distances[i], b);
        denominator += 1 / pow(k_distances[i], b);
    }
    prediction /= denominator;
    delete k_clsfr;
    delete k_distances;
    return prediction;
}
