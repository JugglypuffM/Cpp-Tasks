//
// Created by Jugglyuser on 07.04.2023.
//

#include "List.h"

List::List() {
    size = 0;
    head = nullptr;
}

List::~List() {
    clear();
}

void List::push_back(string data) {
    if (head == nullptr) {
        head = new Node(data);
    } else {
        Node *current = this->head;
        while (current->pNext != nullptr){
            current = current->pNext;
        }
        current->pNext = new Node(data);
    }
    size++;
}

void List::pop_back() {
    removeAt(size-1);
}

void List::push_front(string data) {
    head = new Node(data, this->head);
    size++;
}

void List::pop_front() {
    Node *del = head;
    head = head->pNext;
    delete del;
    size--;
}

void List::insert(string data, int index) {
    if (index == 0)
        push_front(data);
    else {
        reach_the_index(index)->pNext = new Node(data, reach_the_index(index)->pNext);
        size++;
    }
}

void List::removeAt(int index) {
    if (index == 0)
        pop_front();
    else{
        Node *toDelete = reach_the_index(index)->pNext;
        reach_the_index(index)->pNext= toDelete->pNext;
        delete toDelete;
        size--;
    }
}

void List::clear() {
    while (size)
        pop_front();
}

string &List::operator[](const int index) const {
    int counter = 0;
    Node *current = this->head;
    while (current != nullptr) {
        if (counter == index) {
            return current->data;
        }
        current = current->pNext;
        counter++;
    }
}

void List::setSize(int value){
    size = value;
}

Node* List::reach_the_index(int index) const {
    Node *previous = this->head;
    for (int i = 0; i < index - 1; i++)
        previous = previous->pNext;
    return previous;
}

bool List::contains(string data) const {
    if (head == nullptr) {
        return false;
    } else {
        Node *current = this->head;
        while (current->pNext != nullptr){
            if(current->data == data){
                return true;
            }
            current = current->pNext;
        }
    }
    return false;
}

