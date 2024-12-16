#include <iostream>
#include "fstream"
#include "string"
#include "List.h"
#include "HashMap.h"

using namespace std;



int main() {
    HashMap first = HashMap(17, 7);
    HashMap second = HashMap(31, 11);
    HashMap third = HashMap(101, 2);
    int amount, count;
    string current;
    cin >> amount;
    ofstream output(R"(C:\things\CLion\Small Tasks\HashMap\output)");

    count = 0;
    srand(time(nullptr)%12345);
    while(count != amount){
        current = string() + (char)((int)('a') + rand()%26) + (char)((int)('a') + rand()%26) + (char)((int)('a') + rand()%26) + (char)((int)('a') + rand()%26);
        if(!first.contains(current) and !second.contains(current) and !third.contains(current)){
            first.add(current);
            second.add(current);
            third.add(current);
            count++;
        }
    }

    for (int i = 0; i < first.size; ++i) {
        output << first.storage[i].size << endl;
    }
    output << endl;

    for (int i = 0; i < second.size; ++i) {
        output << second.storage[i].size << endl;
    }
    output << endl;

    for (int i = 0; i < third.size; ++i) {
        output << third.storage[i].size << endl;
    }
    first.clear();
    second.clear();
    third.clear();
    return 0;
}
