#include <iostream>
#include "limits"
#include "Objects.h"

double calculateTime(Line l, Plane f) {
    double t = -(f.d + f.a * l.p.x + f.b * l.p.y + f.c * l.p.z) / (f.a * l.v.x + f.b * l.v.y + f.c * l.v.z);
    return (t > 0) ? t : 999999999999999;
}

int main() {
    Cube space(Plane(1, 0, 0, -1), Plane(0, 1, 0, -1), Plane(0, 0, 1, -1));
    Point position;
    Vector speed;
    double time;
    cin >> position.x >> position.y >> position.z;
    cin >> speed.x >> speed.y >> speed.z;
    cin >> time;
    do{
        Line direction = Line(position, speed);
        double times[] = {calculateTime(direction, Plane(1, 0, 0, 0)), calculateTime(direction, Plane(0, 1, 0, 0)),
                          calculateTime(direction, Plane(0, 0, 1, 0)), calculateTime(direction, space.xy),
                          calculateTime(direction, space.yz), calculateTime(direction, space.xz)};

        double mi = *min_element(times, times + 6);
        Plane surface = Plane(0, 0, 0, 0);
        for(int i = 0; i < 6; ++i) {
            if(times[i] == mi) {
                if (i == 0 || i == 3)
                    surface.a = 1;
                else if (i == 1 || i == 4)
                    surface.b = 1;
                else if (i == 2 || i == 5)
                    surface.c = 1;
            }
        }
        if(time - mi < 0) {
            position = Point(direction.p.x + speed.x, direction.p.y + speed.y, direction.p.z + speed.z);
            cout << position.x << " " << position.y << " " << position.z << endl;
            break;
        }
        Vector normal(surface.a, surface.b, surface.c);
        Vector projection = speed - normal * ((speed * normal) / (normal * normal));
        speed = projection*2 - speed;
        time -= mi;
        position = Point(direction.p.x + direction.v.x * mi, direction.p.y + direction.v.y * mi, direction.p.z + direction.v.z * mi);
        cout << position.x << " " << position.y << " " << position.z << endl;
    } while (time >= 0);
}