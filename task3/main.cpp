//
//  main.cpp
//  task3
//
//  Created by Oleg Koptev on 08.11.2020.
//

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#define NUM_THREADS 4

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

struct Triplet {
    Point3D triplet[3];
    bool bCoplanar;
};

int main(int argc, const char * argv[]) {
    
    // 1. Check for correct amount of arguments
    if (argc != 3) {
        cout << "Wrong amount of arguments\n";
        return 1;
    }
    
    // 2. Read all vectors from input file
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
    
    
    // 3. Form an array of all possible triplets
    long amountOfPoints = vectors.size();
    long amountOfTriplets = amountOfPoints * (amountOfPoints - 1) * (amountOfPoints - 2) / 6;
    vector<Triplet> triplets(amountOfTriplets);
    for (auto i = vectors.begin(); i != vectors.end(); ++i) {
        for (auto j = next(i, 1); j != vectors.end(); ++j) {
            for (auto k = next(j, 1); k != vectors.end(); ++k) {
                struct Triplet triplet;
                triplet.triplet[0] = *i;
                triplet.triplet[1] = *j;
                triplet.triplet[2] = *k;
                triplet.bCoplanar = false;
            }
        }
    }
    
    
    
    
    return 0;
}
