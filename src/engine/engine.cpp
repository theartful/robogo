#include "interface.h"

bool go::engine::is_valid_move(const BoardState& board_state, const Action& action){
    Cell cell = board_state(action.x, action.y);
    return is_empty_cell(cell) && !is_suicidal_cell(cell, action.player_index) && !is_dead_cell(cell, action.player_index);

}

bool go::engine::is_empty_cell(Cell cell){
    return (cell & (WHITE_BIT | BLACK_BIT)) == 0;
}

bool go::engine::is_suicidal_cell(Cell cell, uint32_t player){
    return (cell & (SUICIDE_BITS[player]) ) != 0;
}

bool go::engine::is_dead_cell(Cell cell, uint32_t player_index){
    return (cell & (DEAD_BIT | PLAYERS[player_index]) ) != 0;
}

bool go::engine::is_dead_cell(Cell cell){
    return (cell & DEAD_BIT) != 0;
}
