//
// Created by Jugglyuser on 27.03.2023.
//
#include "Vector.h"

using namespace std;

Vector::Vector(double a, double b, double c)
: x(a), y(b), z(c){}

Vector::Vector()
: x(0), y(0), z(0){}

Vector Vector::operator+(Vector v) const {
    return Vector(x + v.x, y + v.y, z + v.z);
}

Vector Vector::operator-(Vector v) const {
    return Vector(x - v.x, y - v.y, z - v.z);
}

Vector Vector::operator*(double a) const {
    return Vector(a*x, a*y, a*z);
}

Vector Vector::operator/(double a) const {
    return Vector(x/a, y/a, z/a);
}

bool Vector::operator==(Vector a) const {
    return ((x == a.x) and (y == a.y) and (z == a.z));
}

double Vector::length() const {
    return sqrt(x*x + y*y + z*z);
}

double Vector::operator*(Vector a) const{
    return (x*a.x + y*a.y + z*a.z);
}

Vector Vector::operator&(Vector a) const {
    return Vector(y*a.z - z*a.y, z*a.x - x*a.z, x*a.y - y*a.x);
}

string Vector::toString() const{
    return (to_string(x) + " " + to_string(y) + " " + to_string(z));
}

Vector Vector::normalize() const {
    double l = this->length();
    if(l == 0){
        return Vector(0, 0, 0);
    } else{
        return Vector(x/l, y/l, z/l);
    }
}

Vector Vector::orthogonal(Vector b) const {
    return Vector(x, y, z)&b;
}

Vector Vector::rotate(Vector a, double angle) {
    if((this->length() == 0) or (a.length() == 0)){
        return Vector(0, 0, 0);
    }
    Vector orth = this->orthogonal(a).normalize();

    double radian = angle*(M_PI/180);
    while(radian < 0)
        radian+=2*M_PI;
    while(radian > 2*M_PI)
        radian-=2*M_PI;

    double matrix[3][3] = {{cos(radian) + (1-cos(radian))*orth.x*orth.x,         (1-cos(radian))*orth.x*orth.y - sin(radian)*orth.z,      (1-cos(radian))*orth.x*orth.z + sin(radian)*orth.y},
                          {(1-cos(radian))*orth.y*orth.x + sin(radian)*orth.z,    cos(radian) + (1-cos(radian))*orth.y*orth.y,             (1-cos(radian))*orth.y*orth.z - sin(radian)*orth.x},
                          {(1-cos(radian))*orth.z*orth.x - sin(radian)*orth.y,   (1-cos(radian))*orth.z*orth.y + sin(radian)*orth.x,       cos(radian) + (1-cos(radian))*orth.z*orth.z}};

    double rx = matrix[0][0]*x + matrix[0][1]*y + matrix[0][2]*z;
    double ry = matrix[1][0]*x + matrix[1][1]*y + matrix[1][2]*z;
    double rz = matrix[2][0]*x + matrix[2][1]*y + matrix[2][2]*z;
    double e = numeric_limits<double>::epsilon();

    return Vector((rx < e) ? 0 : rx, (ry < e) ? 0 : ry, (rz < e) ? 0 : rz);
}
