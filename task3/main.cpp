//
//  main.cpp
//  task3
//
//  Created by Oleg Koptev on 08.11.2020.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

#define NUM_THREADS 4

/**
 * Definition of 3 space lengths
 */
class Point3D {
private:
    float x_;
    float y_;
    float z_;
public:
    Point3D() { x_ = 0; y_ = 0; z_ = 0; }
    Point3D(float x, float y, float z) { x_ = x; y_ = y; z_ = z; }
    float x() { return x_; }
    float y() { return y_; }
    float z() { return z_; }
};

/**
 * Structure containing 3 vectors and boolean value it they are coplanar
 */
struct Triplet {
    Point3D triplet[3];
    bool bCoplanar;
};

/**
 * Information for a thread
 */
typedef struct {
    long start_index;
    long end_index;
    vector<Triplet>* triplets;
} pthrData;

/**
 * Function for a thread, that counts coplanar property for needed amount of triplets
 */
void* threadFunc(void* thread_data) {
    pthrData *data = (pthrData*) thread_data;
    auto start = next(data->triplets->begin(), data->start_index);
    auto end = next(data->triplets->begin(), data->end_index);
    
    for (auto i = start; i != end; ++i) {
        Triplet* triplet = &(*i);
        float value =
            triplet->triplet[0].x() *
                (triplet->triplet[1].y() * triplet->triplet[2].z() -
                 triplet->triplet[1].z() * triplet->triplet[2].y()) +
            triplet->triplet[0].y() *
                (triplet->triplet[1].z() * triplet->triplet[2].x() -
                 triplet->triplet[1].x() * triplet->triplet[2].z()) +
            triplet->triplet[0].z() *
                (triplet->triplet[1].x() * triplet->triplet[2].y() -
                 triplet->triplet[1].y() * triplet->triplet[2].x());
        if (value == 0) {
            triplet->bCoplanar = true;
        }
    }
    
    return NULL;
};

int main(int argc, const char * argv[]) {
    
//  1. Check for correct amount of arguments
    if (argc != 3) {
        cout << "Wrong amount of arguments\n";
        return 1;
    }
    
//  2. Read all vectors from input file
    vector<Point3D> vectors;
    float x, y, z;
    ifstream input (argv[1]);
    if (input.is_open()) {
        while (input >> x >> y >> z) {
            vectors.push_back(Point3D(x, y, z));
        }
        input.close();
    } else {
        cout << "Couldn't open the input file\n";
        return 1;
    }
    
    
//  3. Form an array of all possible triplets
    long amountOfPoints = vectors.size();
    long amountOfTriplets = amountOfPoints * (amountOfPoints - 1) * (amountOfPoints - 2) / 6;
    vector<Triplet> triplets(amountOfTriplets);
    int tripletIndex = 0;
    for (auto i = vectors.begin(); i != vectors.end(); ++i) {
        for (auto j = next(i, 1); j != vectors.end(); ++j) {
            for (auto k = next(j, 1); k != vectors.end(); ++k) {
                struct Triplet triplet;
                triplet.triplet[0] = *i;
                triplet.triplet[1] = *j;
                triplet.triplet[2] = *k;
                triplet.bCoplanar = false;
                
                triplets[tripletIndex] = triplet;
                tripletIndex++;
            }
        }
    }
    
//  4. Calculate coplanar property for all triplets
    if (amountOfTriplets < NUM_THREADS) {
        pthread_t thread;
        pthrData threadData;
        threadData.triplets = &triplets;
        threadData.start_index = 0;
        threadData.end_index = amountOfTriplets;
        pthread_create(&thread, NULL, threadFunc, &threadData);
        pthread_join(thread, NULL);
    } else {
        pthread_t* threads = (pthread_t*) malloc(NUM_THREADS * sizeof(pthread_t));
        pthrData* threadData = (pthrData*) malloc(NUM_THREADS * sizeof(pthrData));
        long shift = ceil((float)amountOfTriplets / (float)NUM_THREADS);
        for (int i = 0; i < NUM_THREADS; i++) {
            threadData[i].triplets = &triplets;
            threadData[i].start_index = i * shift;
            if (i < NUM_THREADS - 1) {
                threadData[i].end_index = (i + 1) * shift;
            } else {
                threadData[i].end_index = amountOfTriplets;
            }
            
            pthread_create(&(threads[i]), NULL, threadFunc, &threadData[i]);
        }
        for(int i = 0; i < NUM_THREADS; i++)
            pthread_join(threads[i], NULL);
    }
    
//  5. Write coplanar triplets
    stringstream outputString;
    for (auto i = triplets.begin(); i != triplets.end(); ++i) {
        Triplet triplet = *i;
        if (!triplet.bCoplanar) { continue; }
        outputString << "(("
            << triplet.triplet[0].x() << ", " << triplet.triplet[0].y() << ", " << triplet.triplet[0].z() << "), ("
            << triplet.triplet[1].x() << ", " << triplet.triplet[1].y() << ", " << triplet.triplet[1].z() << "), ("
            << triplet.triplet[2].x() << ", " << triplet.triplet[2].y() << ", " << triplet.triplet[2].z() << "))\n";
    }
    ofstream output (argv[2]);
    if (output.is_open()) {
        output << outputString.str();
    } else {
        cout << "Error when writing to output file\n";
    }
    
    
    return 0;
}
