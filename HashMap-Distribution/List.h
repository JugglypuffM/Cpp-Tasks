//
// Created by Jugglyuser on 07.04.2023.
//

#include "iostream"
#include "string"

using namespace std;

#ifndef LIST_H
#define LIST_H

class Node {
public:
    Node *pNext;
    string data;
    Node(string data = string(), Node *pNext = nullptr) {
        this->data = data;
        this->pNext = pNext;
    }
};

class List {
public:
    int size;
    Node *head;

    List();
    ~List();
    void push_back(string data);
    void pop_back();
    void push_front(string data);
    void pop_front();
    void insert(string data, int index);
    void removeAt(int index);
    void clear();
    int getSize() const { return size; }
    string &operator[](int index) const;
    void setSize(int);
    Node *reach_the_index(int index) const;
    bool contains(string data) const;
};

#endif //LIST_H
