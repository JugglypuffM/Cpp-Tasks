//
// Created by Jugglyuser on 27.05.2023.
//

#pragma once
#include "string"
#include "iostream"
#include "vector"

using namespace std;

struct Graph {
    int x, o;
    string change;
    int outcomes[3];
    vector<Graph> children;
    Graph(int, int);
    void build(bool);
};

string** fill_table();