#include <iostream>
#include <string>
#include <queue>
#include "Vector.h"

using namespace std;

string to_string (const Vector &v){
    return string() + (char)(v.x + 'a') + (char)(v.y + '1');
}

void print_way(const Vector&begin, const Vector&end, int** table, Vector*dir){
    Vector pos = end;
    string way = to_string(pos);

    while(pos != begin){
        pos = pos - dir[table[(int)pos.x][(int)pos.y]];
        way = to_string(pos) + '\n' + way;
    }

    cout << way << endl;
}

int main(){
    string s, f;
    cin >> s >> f;
    Vector start, finish;
    start.x = s[0] - 'a';
    finish.x = f[0] - 'a';
    start.y = s[1] - '1';
    finish.y = f[1] - '1';
    start.z = finish.z = 0;
    queue<Vector>q;
    q.push(start);
    int** table = new int*[8];
    for (int i = 0; i < 8; ++i) {
        table[i] = new int[8];
        for (int j = 0; j < 8; ++j) {
            table[i][j] = -1;
        }
    }
    Vector dir[8]{{2,1,0}, {2,-1,0}, {1,2,0}, {1,-2,0}, {-1,2,0}, {-1,-2,0}, {-2,1,0}, {-2,-1,0}};
    while(true){
        Vector position = q.front();
        q.pop();
        if(position == finish){
            print_way(start, finish, table, dir);
            break;
        }
        for (int i = 0; i < 8; i++){
            Vector next = position + dir[i];
            if(next.x >= 0 && next.x < 8 && next.y >= 0 && next.y < 8 && table[(int)next.x][(int)next.y] == -1){
                q.push(next);
                table[(int)next.x][(int)next.y] = i;
            }
        }

    }
}