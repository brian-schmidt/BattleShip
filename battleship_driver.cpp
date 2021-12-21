#include<string>
#include<iostream>
#include<fstream>
#include "battleship.h"
#include <time.h>       /* time */
#include <cstdlib>
using namespace std;

int main() { 
    srand(time(NULL));
    BattleShip battle_game;

    
    battle_game.welcome_prompt();
    battle_game.setup_board_1();
    battle_game.setup_board_2();

    
    while (!battle_game.winner()) {
         battle_game.user_turn();
         battle_game.computer_turn();
    }

    return 0;
}
