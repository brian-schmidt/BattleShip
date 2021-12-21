// include the header file so the compiler knows how BattleShip
// is defined in order to compile the member functions
#include "battleship.h"     
#include<string>
#include<iostream>
#include<fstream>
#include<unordered_map>
#include <time.h>       /* time */
#include <cstdlib>
using namespace std;

/**
* BattleShip() - constructor
* initializes p1 and p2 boards and sets all points to "-"
* initializes ships map and stores all 5 ships and their respective number of cells
*/
BattleShip::BattleShip() {
    // init p1_board
    // init p2_board
    for (int i = 0; i < BattleShip::COLUMNS; i++) {
        for (int j = 0; j < BattleShip::ROWS; j++) {
            BattleShip::p1_board[i][j] = '-';
            BattleShip::p2_board[i][j] = '-';
            BattleShip::p2_board_hidden[i][j] = '-';
        }
    }

    // get information for the ships from ship_info.txt 
    ifstream ship_info;
    ship_info.open("ship_info.txt");
    
    string name;
    int cells;
    char indicator;
    int cnt;
    total_cells = 0;        // count up all the cells for checking winner status
    while (ship_info >> name >> cells >> indicator){

        // init all of player #1's ships
        p1_ships[cnt].name = name;
        p1_ships[cnt].cells = cells;
        p1_ships[cnt].indicator = indicator;
        p1_ships[cnt].is_sunken = false;
        p1_ships[cnt].hits = 0;

        // init all of player #2's ships
        p2_ships[cnt].name = name;
        p2_ships[cnt].cells = cells;
        p2_ships[cnt].indicator = indicator;
        p2_ships[cnt].is_sunken = false;
        p2_ships[cnt].hits = 0;

        total_cells+= cells;
        cnt++;
    }

    // init stat structs to store game statistics for both players
    p1_stats.hits = 0;
    p1_stats.misses = 0;
    p1_stats.is_winner = false;

    p2_stats.hits = 0;
    p2_stats.misses = 0;
    p2_stats.is_winner = false;
}

/**
 * 
 */

/**
 * welcome_primpt() - displays an introduction as well as the rules to the user's screen
 * then prompts the user to press enter when they are finished reading the rules...
 */
void BattleShip::welcome_prompt(){
    cout << "**** Welcome to Battleship! ****" << endl;
    cout << "Rules of the Game:" << endl;
    cout << "1. There are 2 players." << endl << "etc..." << endl;
    cout << "Press any key to begin... " << endl;

    // this loop breaks when user presses enter key to start the game
    while (1) {
        if (cin.get() == '\n'){
            break;
        }
    }
    system("clear");
}



/**
 * get_point_address(int row, int column, char* board) - returns the memory address of a certain point on a board.
 * int row - row of desired point
 * int column - column of desired point
 * char* board - the first address of the board 
 */
char* BattleShip::get_point_address(int row, int column, char* board) {
    return (board + (BattleShip::COLUMNS * row) + column);
}

/**
 * update_board(char* point, char newChar) - changes the char value of a given point on a player's board
 * char* point - memory address of the point that needs to be updated
 * newChar - the new value to update the point to
 */
void BattleShip::update_board(char* point, char newChar) {
    *(point) = newChar;   
}

/**
 * display_board() - displays a battleship board
 */
void BattleShip::display_board(char* board){
    cout << "  0 1 2 3 4 5 6 7 8 9" << endl;
    char row_letters[] = {'A','B','C','D','E','F','G','H','I','J'};
    for (int i = 0; i < BattleShip::COLUMNS; i++) {
        cout << row_letters[i] << " ";
        for (int j = 0; j < BattleShip::ROWS; j++) {
            cout << *(board++) << " ";
        }
        cout << endl;
    }
}

/**
 * user_turn() - Prompts the user to choose their 'shot' against the computer
 */
void BattleShip::user_turn() {
    
    while (1) {
        system("clear");
        cout << "      YOUR BOARD" << endl;
        display_board(get_board1());
        cout << endl << "--------------------------" << endl << endl;
        cout << "      YOUR SHOTS" << endl;
        display_board(get_hidden_board());
        string coordinate;
        cout << "Enter your target coordinate: ";
        cin >> coordinate;


        // TEMP TESTING CONDITION
        if (coordinate == "000") {
            p1_stats.hits = total_cells;
            break;
        }

        // check if the user input is valid 
        if (is_valid_coordinate(coordinate)) {
        

            // get the address of this coordinate on player 2's board to check if there is a ship or not
            char* coordinate_address = get_coordinate_address(coordinate, get_board2());

            // check to see if there is a ship there
            bool ship_hit = ship_detected(coordinate_address);
            
            // what to display on the board at the targeted position (* = hit, X = miss)
            char board_indicator;

            // process the shot
            if (ship_hit) {
                board_indicator = '*';

                Ship ship_hit = get_ship(coordinate_address);       // find out which ship is at 
                ship_hit.hits++;        // update ship's hits
                p1_stats.hits++;        // update stats
                if (ship_hit.hits == ship_hit.cells) {
                    ship_hit.is_sunken = true;
                }
                
            } else {
                board_indicator = 'X';
                p1_stats.misses++;
            }
                  
            update_board(get_coordinate_address(coordinate, get_hidden_board()), board_indicator);
            break;
        } 
    }  
}

void BattleShip::register_shot(char* board, string shot_coordinate) {

    // get ptr to the board location that was shot
    char* shot_ptr = get_coordinate_address(shot_coordinate, board);
    

    // determine whether there was a ship at the target location or not...
    bool ship_hit = ship_detected(shot_ptr);

    char board_indicator;        // will store board indicator char ('*' for a hit, 'X' for a miss)

    // branch based on whether there is a ship at the target location or not
    if (ship_hit) {

        board_indicator = '*';  // will indicate a hit on the board

        // find out what ship was hit
        Ship ship = get_ship(shot_ptr);
        ship.hits++;        // increment hit number
        // determine if the ship was sunken by checking if hits == cells
        if (ship.hits == ship.cells) {
            ship.is_sunken = true;
            cout << ship.name << " has been sunken...";
        }
    } else {
        board_indicator = 'X'; 
          
    }
}
/**
 * computer_turn() - Takes a 'shot' on the user's board
 */
void BattleShip::computer_turn() {

    // generate random point on board for computer to shoot at...
    bool valid_shot = false;
    // keep generating random coordinates until one generates that the computer has not taken a shot at yet
    while (!valid_shot) {

        // get random row:
        int row = rand() % 10;
        char row_letter = row + 'a';

        // get random column:
        int column = rand() % 10;
        char column_char = column + '0';

        // join row column into a string coordinate format (Example: 'c8')
        string coordinate = "";
        coordinate.push_back(row_letter);
        coordinate.push_back(column_char);
        cout << "coordinate string: " << coordinate << endl;

        // check to see if computer's coordinate is a hit on p1's board
        char* shot_address = get_coordinate_address(coordinate, get_board1());

        // if the computer hasn't taken a shot at the randomly generated location yet
        if (*(shot_address) != 'M' || '*') {    
            valid_shot = true;      // proceed with the random coordinate as it is valid
            if (ship_detected(shot_address)) {
                update_board(shot_address, '*');        // update board #1 to reflect the computer's shot
                p2_stats.hits++;                        // increment computer's hit counter
            } else {
                update_board(shot_address, 'X');        // update board #1 to reflect the computer's shot
                p2_stats.misses++;                      // increment computer's miss counter
            }  
        }
    }   
}

/**
 * determines if a ship was sunken
 */
void BattleShip::check_sinkage() {

}

char* BattleShip::get_hidden_board() {
    return *p2_board_hidden;
}

/**
 * winner() - checks to verify if either player has won the game by checking the truth value of is_winner member of
 *            the player's stats structure
 */
bool BattleShip::winner(){
    return p1_stats.hits == total_cells || p2_stats.hits == total_cells;
} 

/**
 * returns the Ship at a point on a board
 */
Ship BattleShip::get_ship(char* point) {
    for (int i = 0; i < SHIPS; i++) {
        if (p1_ships[i].indicator == *(point)) {
            return p1_ships[i];
        }
    }
    return p1_ships[0];
}

/**
 * setup_board_1() - places the user's ships (player #1)
 * first prompts the user to select "manual" placement or "auto" placement
 * manual placement allows the user to enter coordinates for specific placements of each ship
 * auto mode will randomly place the ships for the user.
 */
void BattleShip::setup_board_1() {
    cout << "Enter \"manual\" to manually place your ships or" << endl;
    cout << "Enter \"auto\" to have your ships randomly placed by the computer: " << endl;

    string mode;
    while(1) {
        cin >> mode; 
        if (mode == "manual") {
            cout << "manual board setup selected." << endl;
            BattleShip::manual_setup();
            break;
        } else if (mode == "auto") {
            cout << "auto board setup selected." << endl;
            ifstream in_file;
            in_file.open("p2_ships.txt");

            // open the input file for board #2 ship locations
            if (!in_file.fail()){
            string line;
            string ship;
            // read in the input file line by line
            while (getline(in_file, line)) {

            // determine if the current line of input is a ship name
                if (line.length() > 2) {
                     ship = line;    //update ship
                } else {
                    char ship_indicator;
                    if (ship == "cruiser") {
                       ship_indicator = 'r';
                    }  else {
                     ship_indicator = ship.at(0);
                }
                BattleShip::update_board(BattleShip::get_coordinate_address(line, get_board1()), ship_indicator);
            } 
        }
    } else {
        cout << "failed to open file...";
    }
            break;
        } else {
            cout << "Invalid input. Enter \"auto\" or \"maunal\"." << endl;
        }
    }
}    

/**
 * Sets up player #2's ships by reading in data from an input file containing coordinates for each ship
 */
void BattleShip::setup_board_2() {
    
    ifstream in_file;
    in_file.open("p2_ships.txt");

    // open the input file for board #2 ship locations
    if (!in_file.fail()){
        string line;
        string ship;
        // read in the input file line by line
        while (getline(in_file, line)) {

            // determine if the current line of input is a ship name
            if (line.length() > 2) {
                ship = line;    //update ship
            } else {
                char ship_indicator;
                if (ship == "cruiser") {
                    ship_indicator = 'r';
                } else {
                    ship_indicator = ship.at(0);
                }
                BattleShip::update_board(BattleShip::get_coordinate_address(line, get_board2()), ship_indicator);
            } 
        }
    } else {
        cout << "failed to open file...";
    }
}

/**
 * Returns a pointer to board #1's first element
 */
char* BattleShip::get_board1() {
    return *p1_board;
}

/**
 * Returns a pointer to board #2's first element
 */
char* BattleShip::get_board2() {
    return *p2_board;
} 
 
/**
 * Determines whether a ship exists at a given point on a board, determined by the contents of the point's memory address 
 */
bool BattleShip::ship_detected(char* point) {
    return *(point) != '-';
}

/**
bool BattleShip::has_space(char* point, int ship_size){
    // look in each direction on the board to see if there is adequate space for the ship

}
*/

/**
 * Takes a string coordinate and returns the address to that point on the given board
 */
char* BattleShip::get_coordinate_address(string coordinate, char* board) {
    int row = tolower(coordinate.at(0)) - 'a';        // convert row char to int value
    int column = coordinate.at(1) - '0';              // convert column char to int
    return get_point_address(row, column, board);
}

/**
 * manual_setup() - Asks the user for coordinates
 */
void BattleShip::manual_setup() {
    // loop through all the ships
    for (int i = 0; i < BattleShip::SHIPS; i++) {

        // iterate however many cells the current ship has
        for (int j = 0; j < p1_ships[i].cells; j++) {
            system("clear");
            string coordinate;
            cout << "      YOUR BOARD" << endl;
            BattleShip::display_board(get_board1());
            string coordinate_input;
            while (1) {
                cout << "Enter a coordinate for your " << p1_ships[i].name << endl;
                cin >> coordinate_input;
                if (coordinate_input == "000") {
                        goto endloop;
                    }
                if (is_valid_coordinate(coordinate_input)) {
                    BattleShip::update_board(BattleShip::get_coordinate_address(coordinate_input, get_board1()),p1_ships[i].indicator);
                    break;
                }
            }
        }
    }   
    endloop:
        cout << "exiting...";     
}


/**
 * is_valid_coordinate - checks to see if a string is a valid gameboard coordinate  
 */
bool BattleShip::is_valid_coordinate(string coordinate){
    char row_input;
    int column_input;

    // temp
    if (coordinate == "000") {
        return true;
    }

    // check to see if input is two characters long
    if (coordinate.length() != 2) {
        cout << "Input was an invalid length... coordinate should be one letter and one number formatted like so: \'a7\'" << endl;
        return false;
    } else {

        // if input is two characters assign the first to row_input and second to column_input
        row_input = coordinate.at(0);
        column_input = coordinate.at(1) + 0;
    }

    // validate that the row input is a valid letter (a-j)
    if (!isalpha(coordinate.at(0)) || tolower(coordinate.at(0)) < 61|| tolower(coordinate.at(0)) > 106) {
        cout << "Row input was invalid... the row must be a letter a-j";
        return false;
    } else {
        // input passed all the checks
        return true;
    }
}



