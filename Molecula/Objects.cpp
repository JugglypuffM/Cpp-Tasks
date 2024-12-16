//
// Created by Jugglyuser on 05.04.2023.
//
#include "Objects.h"


Plane::Plane(double a, double b, double c, double d)
: a(a), b(b), c(c), d(d){}

Plane::Plane()
: a(0), b(0), c(0), d(0){}

Point::Point(double x, double y, double z)
: x(x), y(y), z(z){}

Point::Point()
: x(0), y(0), z(0){}

Line::Line(Point p, Vector v)
: p(p), v(v){}

Line::Line()
: p(Point()), v(Vector()){}

Cube::Cube(Plane f, Plane s, Plane p)
: xy(f), yz(s), xz(p){}

Cube::Cube()
: xy(Plane()), yz(Plane()), xz(Plane()){}
