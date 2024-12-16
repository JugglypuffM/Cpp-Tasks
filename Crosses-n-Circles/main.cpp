#include <iostream>
#include "Interface.h"
#include "fstream"
#include "Graph.h"

using namespace std;

string** table;

void initiate_table();

int main() {
    initiate_table();
    Interface interface;
    do {
        interface.initiate();
        do {
            interface.make_turn();
            interface.print_state();
        }while(!interface.check_end());
    } while (!interface.quit());
}

void initiate_table(){
    ifstream fin;
    fin.open(R"(C:\things\CLion\X&O\table.txt)");
    if (!fin || (fin.peek() == ifstream::traits_type::eof())){
        ofstream fout;
        fout.open(R"(C:\things\CLion\X&O\table.txt)");
        table = fill_table();
        for (int i = 0; i < 512; ++i) {
            for (int j = 0; j < 512; ++j) {
                if (!table[i][j].empty()) fout << to_string(i) + " " + to_string(j) + " " + table[i][j] << endl;
            }
        }
    } else{
        table = new string*[512];
        for (int i = 0; i < 512; ++i) {
            table[i] = new string[512];
        }
        while (!fin.eof()){
            int i, j;
            fin >> i >> j;
            fin >> table[i][j];
        }
    }
}