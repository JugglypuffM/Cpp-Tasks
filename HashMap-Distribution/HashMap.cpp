//
// Created by Jugglyuser on 07.04.2023.
//

#include "HashMap.h"

using namespace std;

HashMap::HashMap(int s, int b) {
    size = s;
    base = b;
    storage = new List[s];
    amount = 0;
}

void HashMap::add(string s) {
    if(this->contains(s)){
        return;
    }else{
        storage[hash(s)].push_back(s);
        amount++;
    }
}

int HashMap::hash(string s) const {
    int result = 0;
    for(int i = 0; i < s.size(); ++i)
        result+= ((int)s[i]) * pow(base, i);
    result %= size;
    return result;
}

HashMap::~HashMap() {
    clear();
}

void HashMap::clear() {
    while (size){
        storage[size-- -1].clear();
    }
}

bool HashMap::contains(string s) const {
    return storage[hash(s)].contains(s);
}


