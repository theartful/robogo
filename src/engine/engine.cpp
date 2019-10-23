#include <cmath>
#include "interface.h"
#include "liberties.h"

bool go::engine::is_valid_move(const BoardState &board_state, const Action &action)
{
    Cell cell = board_state(action.x, action.y);
    return is_empty_cell(cell) && !is_suicidal_cell(cell, action.player_index) && !is_dead_cell(cell, action.player_index);
}

bool go::engine::make_move(GameState &game_state, const Action &action)
{
    if (game_state.player_turn == action.player_index && make_move(game_state.board_state, action))
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

bool go::engine::make_move(BoardState &board_state, const Action &action)
{
    if (is_valid_move(board_state, action))
    {
        board_state(action.x, action.y) = PLAYERS[action.player_index];
        return true;
    }
    else
    {
        return false;
    }
}

void go::engine::update_dead_cells(BoardState &board_state)
{
    for (uint32_t i = 0; i < board_state.MAX_BOARD_SIZE; i++)
    {
        for (uint32_t j = 0; j < board_state.MAX_BOARD_SIZE; j++)
        {
            if (!is_empty_cell(board_state(i, j)) && (count_liberties(board_state, i, j) == 0))
            {
                mark_dead(board_state, i, j);
            }
        }
    }
}

void go::engine::update_suicide_cells(BoardState &board_state)
{
    for (uint32_t i = 0; i < BoardState::MAX_BOARD_SIZE; i++)
    {
        for (uint32_t j = 0; j < BoardState::MAX_BOARD_SIZE; j++)
        {
            if (is_empty_cell(board_state(i, j)))
            {
                if (simulate_suicide(board_state, i, j, Cell::BLACK))
                    board_state(i, j) = Cell::SUICIDE_BLACK;
                else if (simulate_suicide(board_state, i, j, Cell::WHITE))
                    board_state(i, j) = Cell::SUICIDE_WHITE;
            }
        }
    }
}

bool go::engine::simulate_suicide(BoardState &board_state, uint32_t i, uint32_t j, Cell c)
{
    board_state(i, j) = c;
    if (count_liberties(board_state, i, j) == 0)
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

void go::engine::mark_dead(BoardState &board_state, uint32_t i, uint32_t j)
{
    board_state(i, j) = board_state(i, j) | DEAD_BIT;
}

bool go::engine::is_empty_cell(Cell cell)
{
    return (cell & (WHITE_BIT | BLACK_BIT)) == 0;
}

bool go::engine::is_suicidal_cell(Cell cell, uint32_t player)
{
    return (cell & (SUICIDE_BITS[player])) != 0;
}

bool go::engine::is_dead_cell(Cell cell, uint32_t player_index)
{
    return (cell & (DEAD_BIT | PLAYERS_BITS[player_index])) != 0;
}

bool go::engine::is_dead_cell(Cell cell)
{
    return (cell & DEAD_BIT) != 0;
}

void go::engine::calculate_score(const BoardState &boardState, Player &white_player, Player &black_player)
{
    uint32_t white_territory_score = 0, black_territory_score = 0, score_temp = 0;

    //To detect wether the traversed territory belong to which side, "01" for white and "10" for black
    unsigned char white = CellBits::WHITE_BIT;
    unsigned char black = CellBits::BLACK_BIT;
    unsigned char territory_type;                      // if the output of the traversed territory was "11" the it was a false territory
    bool visited[BoardState::MAX_NUM_CELLS] = {false}; // to avoid starting traversing a new territory from an already traversed empty cell

    //Traversing the board to detect any start of any territory
    for (uint32_t i = 0; i < boardState.MAX_BOARD_SIZE; i++)
    {
        for (uint32_t j = 0; j < boardState.MAX_BOARD_SIZE; j++)
        {
            territory_type = 0b00000000;
            score_temp = 0;
            if (is_empty_cell(boardState(i, j)) && !visited[boardState.index(i, j)])
                score_temp = territory_points(boardState, territory_type, i, j, visited);

            //XNORing the territory_type to figure out the output of the traversed territory
            if (territory_type == white)
                white_territory_score += score_temp;
            else if (territory_type == black)
                black_territory_score += score_temp;
        }
    }

    //Updating scores
    white_player.total_score = white_territory_score + white_player.number_alive_stones + white_player.number_captured_enemies;
    black_player.total_score = black_territory_score + black_player.number_alive_stones + black_player.number_captured_enemies;
}

uint32_t go::engine::territory_points(const BoardState boardState, unsigned char &territory_type, uint32_t x, uint32_t y, bool *visited)
{

    uint32_t score = 0;
    unsigned char white = CellBits::WHITE_BIT;
    unsigned char black = CellBits::BLACK_BIT;
    visited[boardState.index(x, y)] = true;

    // Checking upper cell territory
    if (y != 0)
    {
        if (is_empty_cell(boardState(x, y - 1)) && !visited[boardState.index(x, y - 1)])
            score += territory_points(boardState, territory_type, x, y - 1, visited);
        else if (boardState(x, y - 1) == Cell::WHITE || boardState(x, y - 1) == Cell::DEAD_WHITE)
            territory_type |= white;
        else if (boardState(x, y - 1) == Cell::BLACK || boardState(x, y - 1) == Cell::DEAD_BLACK)
            territory_type |= black;
    }

    // Checking right cell territory
    if (x != boardState.MAX_BOARD_SIZE - 1)
    {
        if (is_empty_cell(boardState(x + 1, y)) && !visited[boardState.index(x + 1, y)])
            score += territory_points(boardState, territory_type, x + 1, y, visited);
        else if (boardState(x + 1, y) == Cell::WHITE || boardState(x + 1, y) == Cell::DEAD_WHITE)
            territory_type |= white;
        else if (boardState(x + 1, y) == Cell::BLACK || boardState(x + 1, y) == Cell::DEAD_BLACK)
            territory_type |= black;
    }

    // Checking bottom cell territory
    if (y != boardState.MAX_BOARD_SIZE - 1)
    {
        if (is_empty_cell(boardState(x, y + 1)) && !visited[boardState.index(x, y + 1)])
            score += territory_points(boardState, territory_type, x, y + 1, visited);
        else if (boardState(x, y + 1) == Cell::WHITE || boardState(x, y + 1) == Cell::DEAD_WHITE)
            territory_type |= white;
        else if (boardState(x, y + 1) == Cell::BLACK || boardState(x, y + 1) == Cell::DEAD_BLACK)
            territory_type |= black;
    }

    // Checking left cell territory
    if (x != 0)
    {
        if (is_empty_cell(boardState(x - 1, y)) && !visited[boardState.index(x - 1, y)])
            score += territory_points(boardState, territory_type, x - 1, y, visited);
        else if (boardState(x - 1, y) == Cell::WHITE || boardState(x - 1, y) == Cell::DEAD_WHITE)
            territory_type |= white;
        else if (boardState(x - 1, y) == Cell::BLACK || boardState(x - 1, y) == Cell::DEAD_BLACK)
            territory_type |= black;
    }

    score++;
    return score;
}