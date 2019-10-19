#include <cmath>
#include "interface.h"
#include "liberties.h"

bool go::engine::is_valid_move(const BoardState& board_state, const Action& action)
{
    Cell cell = board_state(action.x, action.y);
    return is_empty_cell(cell) && !is_suicidal_cell(cell, action.player_index) && !is_dead_cell(cell, action.player_index);

}

bool go::engine::make_move(GameState& game_state, const Action& action)
{
    if(game_state.player_turn == action.player_index && make_move(game_state.board_state, action))
    {
        game_state.player_turn = 1 - action.player_index;
        game_state.number_played_moves++;

        return true;
    }
    else
    {
        return false;
    }
}

bool go::engine::make_move(BoardState& board_state, const Action& action){
    if(is_valid_move(board_state, action))
    {
        board_state(action.x, action.y) = PLAYERS[action.player_index];
        return true;
    }
    else
    {
        return false;
    }
}

void go::engine::update_dead_cells(BoardState& board_state)
{
    for(uint32_t i = 0;i < board_state.MAX_BOARD_SIZE;i++)
    {
        for(uint32_t j = 0;j < board_state.MAX_BOARD_SIZE;j++)
        {
            if(!is_empty_cell(board_state(i, j)) && (count_liberties(board_state, i, j) == 0) )
            {
                mark_dead(board_state, i , j);
            }
        }
    }
}

void go::engine::update_suicide_cells(BoardState& board_state)
{
    for(uint32_t i = 0;i < BoardState::MAX_BOARD_SIZE;i++)
    {
        for(uint32_t j = 0;j < BoardState::MAX_BOARD_SIZE;j++)
        {
            if(is_empty_cell(board_state(i, j))){
                if(simulate_suicide(board_state ,i, j, Cell::BLACK))
                    board_state(i, j) = Cell::SUICIDE_BLACK;
                else
                    if(simulate_suicide(board_state ,i, j, Cell::WHITE))
                        board_state(i, j) = Cell::SUICIDE_WHITE;
            }
        }
    }

}

bool go::engine::simulate_suicide(BoardState& board_state, uint32_t i, uint32_t j, Cell c){
    board_state(i, j) = c;
    if(count_liberties(board_state, i, j) == 0)
    {
        board_state(i, j) = Cell::EMPTY;
        return true;
    }
    else
    {
        board_state(i, j) = Cell::EMPTY;
        return false;
    }
}

void go::engine::mark_dead(BoardState& board_state, uint32_t i, uint32_t j)
{
    board_state(i, j) = board_state(i, j) | DEAD_BIT;
}

bool go::engine::is_empty_cell(Cell cell)
{
    return (cell & (WHITE_BIT | BLACK_BIT)) == 0;
}

bool go::engine::is_suicidal_cell(Cell cell, uint32_t player)
{
    return (cell & (SUICIDE_BITS[player]) ) != 0;
}

bool go::engine::is_dead_cell(Cell cell, uint32_t player_index)
{
    return (cell & (DEAD_BIT | PLAYERS_BITS[player_index]) ) != 0;
}

bool go::engine::is_dead_cell(Cell cell)
{
    return (cell & DEAD_BIT) != 0;
}
