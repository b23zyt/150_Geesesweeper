#include <iostream>
#include "geesespotter_lib.h"

char *create_board(std::size_t x_dim, std::size_t y_dim);
void clean_board(char *board);
void print_board(char *board, std::size_t x_dim, std::size_t y_dim);
void hide_board(char *board, std::size_t x_dim, std::size_t y_dim);
int mark(char *board, std::size_t x_dim, std::size_t y_dim, std::size_t x_loc, std::size_t y_loc);
void compute_neighbours(char *board, std::size_t x_dim, std::size_t y_dim);
bool is_game_won(char *board, std::size_t x_dim, std::size_t y_dim);
int reveal(char *board, std::size_t x_dim, std::size_t y_dim, std::size_t x_loc, std::size_t y_loc);

char *create_board(std::size_t x_dim, std::size_t y_dim){
    char *board{new char[x_dim * y_dim]{}};
    for (std::size_t i = 0; i < x_dim * y_dim; ++i){
        board[i] = 0; 
    }
    return board;
}

void clean_board(char *board){
    delete[] board;
    board = nullptr;
}

void print_board(char *board, std::size_t x_dim, std::size_t y_dim){
    for (std::size_t row = 0; row < y_dim; ++row) {
        for (std::size_t col = 0; col < x_dim; ++col) {
            char mask{board[row * x_dim + col]};
            char print{};
            bool is_hidden{(mask & 0x20) == 0x20};
            bool is_marked{(mask & 0x10) == 0x10};
            char value{mask & 0xf};

            if(is_marked){
                print = 'M';
            }

            else if(is_hidden){
                print = '*';
            }
            
            else{
                print = '0' + value;
            }
            std::cout << print;
        }
        std::cout << std::endl;
    }
}

void hide_board(char *board, std::size_t x_dim, std::size_t y_dim){
    for (std::size_t row = 0; row < y_dim; ++row){
        for (std::size_t col = 0; col < x_dim; ++col) {
            board[row * x_dim + col] |= 0x20;  //100000
        }
    }
}

int mark(char *board, std::size_t x_dim, std::size_t y_dim, std::size_t x_loc, std::size_t y_loc){
    std::size_t index{y_loc * x_dim + x_loc};  // the index of the location 
    if ((board[index] & 0x20) == 0){  //if bit 6 is 0 (revealed)
        return 2;
    }
    else{
        board[index] ^= 0x10; //unmarked -> mark / marked -> unmark
        return 0;
    }
}


void compute_neighbours(char *board, std::size_t x_dim, std::size_t y_dim){
    for (std::size_t row = 0; row < y_dim; ++row) {
        for (std::size_t col = 0; col < x_dim; ++col) {
            char mask{board[row * x_dim + col]};
            std::size_t index{row * x_dim + col};
            if ((mask & 0xf) != 0x9){
                char count = 0;
                for (int i = -1; i <= 1; i++){
                    for (int j = -1; j <= 1; j++){
                        if ((i == 0) && (j == 0)){
                            continue;
                        }
                        int nb_r{row + i};
                        int nb_c{col + j};
                        if ((nb_r >= 0) && (nb_r < y_dim) && (nb_c >= 0) && (nb_c < x_dim)){
                            char nb_musk{board[nb_r*x_dim + nb_c]};
                            if ((nb_musk & 0xf) == 0x9){
                                count ++;
                            }
                        }

                    }
                }
                board[index] &= 0xf0; //clear the last 4 digits
                board[index] |= count; //set the last 4 digits
            }
        }
    }
}

bool is_game_won(char *board, std::size_t x_dim, std::size_t y_dim){
    for (std::size_t row = 0; row < y_dim; ++row) {
        for (std::size_t col = 0; col < x_dim; ++col) {
            char mask = board[row * x_dim + col];
            if (((mask & 0x20) == 0x20) && ((mask & 0xf) != 0x9)){ //hidden and not a goose (win when all the none goose field are revealed)
                return false;
            }
        }
    }
    return true;
}

int reveal(char *board, std::size_t x_dim, std::size_t y_dim, std::size_t x_loc, std::size_t y_loc){
    char mask = board[y_loc*x_dim + x_loc];
    std::size_t index = y_loc * x_dim + x_loc;

    bool is_marked{(mask & 0x10) == 0x10};  
    bool is_reveal{(mask & 0x20) == 0x00};
    bool is_goose{(mask & 0xf) == 0x9};
    if (is_marked){
        return 1;
    }
    else if(is_reveal){ 
        return 2;
    }
    else if(is_goose){ //already not reveal(hidden 1) and unmarked(0)
        board[index] &= 0x1f; //set bit 6 to 0 to reveal the field 
        return 9;
    }

    else if (((mask & 0x0f) == 0x0)){ //if the field is empty
        board[index] &= 0x1f; //set bit 6 to 0 to reveal the field
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0){
                    continue;
                }
                int nb_x = x_loc + j;
                int nb_y = y_loc + i;
                char nb_mask = board[nb_y * x_dim + nb_x];
                std::size_t nb_index = nb_y * x_dim + nb_x;
                if (nb_x >= 0 && nb_x < x_dim && nb_y >= 0 && nb_y < y_dim && ((nb_mask & 0x10) == 0x0)) {
                    board[nb_index] &= 0x1f; //set bit 6 to 0
                }
            }
        }
    }
    else {
        board[index] &= 0x1f; //set bit 6 to to 0 to reveal the field
    }
    return 0;
}