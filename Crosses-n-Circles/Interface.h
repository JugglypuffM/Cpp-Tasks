//
// Created by Jugglyuser on 23.05.2023.
//
#pragma once
#include "iostream"
#include "string"

using namespace std;

class Game{
    struct Player{
        bool human;
    };
    int x, o, difficulty;
    Game::Player x_player, o_player;
    Game::Player* current_turn;
public:
    void set_options(int, bool, bool);
    void do_turn(string);
    int check_winner();
    bool check_emptiness(int, int);
    string return_state();
    Game::Player* whose_turn();
    string ai_decision();
};

class Interface {
    Game game;
    string input;
public:
    void initiate();
    void make_turn();
    void print_state();
    bool check_end();
    bool quit();
};

extern string** table;