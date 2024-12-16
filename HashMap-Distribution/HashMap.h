//
// Created by Jugglyuser on 07.04.2023.
//

#include "iostream"
#include "string"
#include "cmath"
#include "List.h"

using namespace std;

#ifndef HASHMAP_H
#define HASHMAP_H


class HashMap {
public:
    List* storage;
    int size, base, amount;
    HashMap(int, int);
    ~HashMap();
    void clear();
    void add(string);
    int hash(string) const;
    bool contains(string) const;
};


#endif //HASHMAP_H