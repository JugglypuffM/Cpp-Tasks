//
// Created by Jugglyuser on 27.03.2023.
//

#include "iostream"
#include "cmath"
#include "string"
#include "limits"

using namespace std;

#ifndef VECTOR_H
#define VECTOR_H

class Vector{
public:
    double x, y, z;

    Vector(double, double, double);
    Vector();
    Vector operator +(Vector) const;
    Vector operator -(Vector) const;
    Vector operator *(double) const;
    Vector operator /(double) const;
    bool operator==(Vector) const;
    double length() const;
    double operator *(Vector) const;
    Vector operator &(Vector) const;
    string toString() const;
    Vector normalize() const;
    Vector orthogonal(Vector) const;
    Vector rotate(Vector, double);

};

#endif //VECTOR_H