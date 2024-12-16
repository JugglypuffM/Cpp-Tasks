//
// Created by Jugglyuser on 23.05.2023.
//

#include "Interface.h"

void Interface::initiate() {
    bool x_player, o_player;
    int difficulty;
    do {
        cout << "Please choose your shape(X, O or M):" << endl;
        cin >> input;
        if (input == "X") {
            x_player = true;
            break;
        } else if (input == "O") {
            o_player = true;
            break;
        }else if (input == "M") {
            x_player = true;
            o_player = true;
            break;
        }
        cout << "Wrong shape" << endl;
    } while (true);
    if(input != "M") {
        do {
            cout << "Please choose difficulty(0-3):" << endl;
            cin >> input;
            if (input == "0" || input == "1" || input == "2" || input == "3") {
                difficulty = stoi(input);
                break;
            }
            cout << "No such difficulty" << endl;
        } while (true);
    } else{
        difficulty = -1;
    }
    game.set_options(difficulty, x_player, o_player);
}

bool Interface::quit() {
    do {
        cout << "Do you want to quit?(Y/n)" << endl;
        cin >> input;
        if (input == "n") return false;
        return true;
    } while (true);
}

void Interface::make_turn() {
    string decision;
    char a, b;
    if(game.whose_turn()->human){
        do {
            cout << "Make your turn:" << endl;
            cin >> input;
            b = input[0]; a = input[1];
            if(((a == '1' || a == '2' || a == '3') &&
               (b == 'A' || b == 'B' || b == 'C')) &&
                game.check_emptiness(a - (int)'0' - 1, b - (int)'A')){
                decision = string() + (char)(a - 1) + to_string(b - (int)'A');
                break;
            }
            cout << "Turn written incorrectly or position is already occupied" << endl;
        } while (true);

    } else{
        cout << "Computer's turn:" << endl;
        decision = "-1";
    }
    game.do_turn(decision);
}

void Interface::print_state() {
    string state = game.return_state();
    cout << state.substr(0, 3) << endl << state.substr(3, 3) << endl << state.substr(6, 3) << endl;
}

bool Interface::check_end() {
    int winner = game.check_winner();
    if (winner == 1){
        cout << "X's have won!" << endl;
        return true;
    }else if (winner == 2){
        cout << "O's have won!" << endl;
        return true;
    }else if(winner == 0){
        cout << "Draw" << endl;
        return true;
    }else{
        return false;
    }
}

void Game::set_options(int d, bool xp, bool op) {
    difficulty = d;
    x_player.human = xp;
    o_player.human = op;
    current_turn = &this->x_player;
    x = o = 0;
}

void Game::do_turn(string decision) {
    if(decision == "-1"){
        decision = ai_decision();
    }
    if(current_turn == &x_player){
        x = x | (1 << ((decision[0] - (int)'0')*3 + decision[1] - (int)'0'));
        current_turn = &o_player;
    }else{
        o = o | (1 << ((decision[0] - (int)'0')*3 + decision[1] - (int)'0'));
        current_turn = &x_player;
    }

}

int Game::check_winner(){
    int win_states[8] = {448, 292, 273, 146, 84, 73, 56, 7};
    for (int i = 0; i < 8; ++i) {
        if((x & win_states[i]) == win_states[i]) return 1;
        else if ((o & win_states[i]) == win_states[i]) return 2;
        else if((x|o) == 511) return 0;
    }
    return -1;
}

bool Game::check_emptiness(int f, int s) {
    return (((x & (1 << (f*3 + 1*s))) == 0) && (((o & (1 << (f*3 + 1*s))) == 0)));
}

string Game::return_state() {
    string result = "_________";
    for (int i = 0; i < 9; ++i) {
        if((x & (1 << i)) != 0){
            result[i] = 'X';
        }else if((o & (1 << i)) != 0){
            result[i] = 'O';
        }
    }
    return result;
}

Game::Player *Game::whose_turn() {
    return current_turn;
}

string Game::ai_decision() {
    string decision;
    if (difficulty == 0){
        srand(time(nullptr)%12345);
        do {
            decision = string() + (char)(rand()%3 + '0') + (char)(rand()%3 + '0');
        } while(!check_emptiness(decision[0] - (int)'0', decision[1] - (int)'0'));
    }else if(difficulty == 1) decision = table[x][o].substr(4, 2);
    else if(difficulty == 2) decision = table[x][o].substr(2, 2);
    else if(difficulty == 3) decision = table[x][o].substr(0, 2);
    return decision;
}
