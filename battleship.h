// These directives cause the program to skip this header file
// if it is encountered again in another header file that contains this one
#ifndef BATTLESHIP_H
#define BATTLESHIP_H
#include<unordered_map>
#include<string>
#include<vector>
#include <time.h>       /* time */
#include <cstdlib>
using namespace std;

struct Stats{
    int hits;
    int misses;
    bool is_winner;
};

struct Ship{
    string name;
    int cells;
    char indicator;
    int hits;
    bool is_sunken;
};


// This header file contains the interface of the battleship class
class BattleShip{
     public:
        BattleShip();                   // constructor

        void play_game();
        Ship get_ship(char* point);
        void welcome_prompt();          // will display game rules to player
        void user_turn();               // proccesses a 'shot' from the user against the computer
        void computer_turn();           // proccesses a 'shot' from the computer against the player
        void setup_board_1();           // will ask user to choose ship locations then initialize board with ships
        void setup_board_2();           // sets up board #2 by reading in positions from an in file
        void update_board(char* address, char newChar);            // will update the board 
        void display_board(char* board);            // displays a board that has it's first element at char* board
        void register_shot(char* board, string shot_coordinate);      // prompts the user to take a shot
        void check_sinkage();     // determines whether a hit ship is sunken
        bool winner();               // determines whether a winner exists or not
        char* get_board1();             // returns pointer to first element in board #1
        char* get_board2();             // return pointer to first element in board #2
        char* get_hidden_board();       // returns pointer to first element in hidden board
        void manual_setup();            // allows the user to choose each ship location manually
        bool ship_detected(char* point);           // determines if a ship is located at a given point
        char* get_point_address(int row, int column, char* board);         // returns a memory address for point (row, column)
        bool has_space(char* point, int ship_size);             // checks to see if there is space to place a ship at a given location on the board

        const static int ROWS = 10;
        const static int COLUMNS = 10;      
        const static int SHIPS = 5;
        
        void update_board_2(int row, int column, char newChar);
        char* get_north();
        char* get_south();
        char* get_east();
        char* get_west();

        char* get_coordinate_address(string coordinate, char* board);
        bool is_valid_coordinate(string coordinate);        // verifies if a coordinate is valid (for checking user input)
        struct Stats p1_stats;
        struct Stats p2_stats;
        struct Ship p1_ships[SHIPS];
        struct Ship p2_ships[SHIPS];
        string get_coordinate_input();
    private:
        char p1_board[ROWS][COLUMNS];           // game board for player 1
        char p2_board[ROWS][COLUMNS];           // game board for player 2
        char p2_board_hidden[ROWS][COLUMNS];    // game board to store all of player 1's hits and misses 
        int total_cells;
        
        
};



#endif