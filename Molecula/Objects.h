//
// Created by Jugglyuser on 05.04.2023.
//

#include "iostream"
#include "Vector.h"
#include "cmath"
#include "string"
#include "limits"

using namespace std;

#ifndef MOLECULA_OBJECTS_H
#define MOLECULA_OBJECTS_H

struct Plane{
    double a, b, c ,d;
    Plane(double, double, double, double);
    Plane();
};

struct Point{
    double x, y, z;
    Point(double, double, double);
    Point();
};

struct Line{
    Point p;
    Vector v;
    Line(Point, Vector);
    Line();
};

struct Cube{
    Plane xy, yz, xz;
    Cube(Plane, Plane, Plane);
    Cube();
};

#endif //MOLECULA_OBJECTS_H
