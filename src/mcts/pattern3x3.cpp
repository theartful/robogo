#include "pattern3x3.h"
#include <algorithm>
#include <fstream>
#include <iostream>

namespace go
{
namespace mcts
{

enum class PatternSymbols : char
{
	BLACK = 'X',
	WHITE = 'O',
	EMPTY = '.',
	BLACK_OR_EMPTY = 'x',
	WHITE_OR_EMPTY = 'o',
	STONE_OR_EMPTY = '?',
	BORDER = '-',
	INVALID = '@'
};

PatternTable global_pattern_table{};

static inline PatternSymbols get_pattern_symbol(char c)
{
	const std::array pattern_symbols = {
	    PatternSymbols::BLACK,          PatternSymbols::WHITE,
	    PatternSymbols::EMPTY,          PatternSymbols::BLACK_OR_EMPTY,
	    PatternSymbols::WHITE_OR_EMPTY, PatternSymbols::STONE_OR_EMPTY,
	    PatternSymbols::BORDER};

	auto it = std::find(
	    pattern_symbols.begin(), pattern_symbols.end(),
	    static_cast<PatternSymbols>(c));
	if (it == pattern_symbols.end())
		return PatternSymbols::INVALID;
	else
		return static_cast<PatternSymbols>(c);
}

PatternHash get_pattern_hash(const Pattern& pattern)
{
	PatternHash hash = 0;
	uint32_t offset = 0;
	for (auto cell : pattern)
	{
		hash |= (static_cast<unsigned char>(cell) << offset);
		offset += 2;
	}
	return hash;
}

static inline void expand_and_register(
    PatternTable& pattern_table,
    const std::array<PatternSymbols, 8>& pattern_code, uint32_t pattern_id,
    size_t idx, Pattern& pattern)
{
	if (idx >= pattern_code.size())
	{
		PatternHash hash = get_pattern_hash(pattern);
		pattern_table[hash] = pattern_id;
		return;
	}
	PatternSymbols symbol = pattern_code[idx];
	switch (symbol)
	{
	case PatternSymbols::BLACK:
		pattern[idx] = engine::Cell::BLACK;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		break;
	case PatternSymbols::WHITE:
		pattern[idx] = engine::Cell::WHITE;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		break;
	case PatternSymbols::EMPTY:
		pattern[idx] = engine::Cell::EMPTY;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		break;
	case PatternSymbols::BLACK_OR_EMPTY:
		pattern[idx] = engine::Cell::BLACK;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		pattern[idx] = engine::Cell::EMPTY;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		break;
	case PatternSymbols::WHITE_OR_EMPTY:
		pattern[idx] = engine::Cell::WHITE;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		pattern[idx] = engine::Cell::EMPTY;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		break;
	case PatternSymbols::STONE_OR_EMPTY:
		pattern[idx] = engine::Cell::BLACK;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		pattern[idx] = engine::Cell::WHITE;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		pattern[idx] = engine::Cell::EMPTY;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		break;
	case PatternSymbols::BORDER:
		pattern[idx] = engine::Cell::BORDER;
		expand_and_register(
		    pattern_table, pattern_code, pattern_id, idx + 1, pattern);
		break;
	case PatternSymbols::INVALID:
	default:
		std::cerr << "Error registering a pattern. This shouldn't happen!\n";
	}
}

// 0 1 2       2 4 7
// 3 - 4  <=>  1 - 6
// 5 6 7       0 3 5
static inline std::array<PatternSymbols, 8>
rotate_pattern_code(const std::array<PatternSymbols, 8>& pattern_code)
{
	std::array<PatternSymbols, 8> rotated_pattern_code;
	std::array<uint32_t, 8> remapping = {2, 4, 7, 1, 6, 0, 3, 5};
	for (size_t i = 0; i < pattern_code.size(); i++)
		rotated_pattern_code[i] = pattern_code[remapping[i]];
	return rotated_pattern_code;
}

// 0 1 2       2 1 0
// 3 - 4  <=>  4 - 3
// 5 6 7       7 6 5
static inline std::array<PatternSymbols, 8>
flip_pattern_code(const std::array<PatternSymbols, 8>& pattern_code)
{
	std::array<PatternSymbols, 8> flipped_pattern_code;
	std::array<uint32_t, 8> remapping = {2, 1, 0, 4, 3, 7, 6, 5};
	for (size_t i = 0; i < pattern_code.size(); i++)
		flipped_pattern_code[i] = pattern_code[remapping[i]];
	return flipped_pattern_code;
}

static inline std::array<PatternSymbols, 8>
replace_players(const std::array<PatternSymbols, 8>& pattern_code)
{
	const std::unordered_map<PatternSymbols, PatternSymbols> remapping{
	    {PatternSymbols::BLACK, PatternSymbols::WHITE},
	    {PatternSymbols::WHITE, PatternSymbols::BLACK},
	    {PatternSymbols::BLACK_OR_EMPTY, PatternSymbols::WHITE_OR_EMPTY},
	    {PatternSymbols::WHITE_OR_EMPTY, PatternSymbols::BLACK_OR_EMPTY},
	    {PatternSymbols::STONE_OR_EMPTY, PatternSymbols::STONE_OR_EMPTY},
	    {PatternSymbols::EMPTY, PatternSymbols::EMPTY},
	    {PatternSymbols::BORDER, PatternSymbols::BORDER}};
	std::array<PatternSymbols, 8> reversed_pattern_code;
	for (size_t i = 0; i < pattern_code.size(); i++)
		reversed_pattern_code[i] = remapping.at(pattern_code[i]);
	return reversed_pattern_code;
}

static inline void register_pattern(
    PatternTable& pattern_table, std::array<PatternSymbols, 8>& pattern_code,
    uint32_t pattern_id)
{
	auto register_flips_and_rotations = [&]() {
		Pattern pattern{};
		auto register_rotations = [&]() {
			for (size_t i = 0; i < pattern_code.size(); i++)
			{
				expand_and_register(
				    pattern_table, pattern_code, pattern_id, 0, pattern);
				pattern_code = rotate_pattern_code(pattern_code);
			}
		};
		register_rotations();
		pattern_code = flip_pattern_code(pattern_code);
		register_rotations();
	};
	register_flips_and_rotations();
	pattern_code = replace_players(pattern_code);
	register_flips_and_rotations();
}

void load_patterns_file(const std::string& file_name)
{
	std::fill(
	    global_pattern_table.begin(), global_pattern_table.end(), NO_PATTERN);
	std::ifstream pattern_file;
	pattern_file.open(file_name);
	std::string line;

	std::array<PatternSymbols, 8> pattern_code;
	size_t count = 0;
	uint32_t pattern_id = 0;

	bool skipped_4 = false;
	while (std::getline(pattern_file, line))
	{
		for (char c : line)
		{
			if (c == '#')
				break;
			PatternSymbols symbol = get_pattern_symbol(c);
			if (symbol != PatternSymbols::INVALID)
			{
				if (count == 4 && !skipped_4)
				{
					skipped_4 = true;
					if (symbol != PatternSymbols::EMPTY)
					{
						std::cerr << "Error reading patterns file!\n";
						return;
					}
					continue;
				}
				pattern_code[count++] = symbol;
				skipped_4 = false;
			}
			if (count == 8)
			{
				count = 0;
				register_pattern(
				    global_pattern_table, pattern_code, pattern_id++);
			}
		}
	}
	std::cerr << "Registered " << pattern_id << " patterns!\n";
}

// 0 1 2
// 3 - 4
// 5 6 7
PatternHash get_pattern_hash(const engine::BoardState& state, uint32_t pos)
{
	const auto neighbors = {
	    pos - engine::BoardState::EXTENDED_BOARD_SIZE - 1,
	    pos - engine::BoardState::EXTENDED_BOARD_SIZE,
	    pos - engine::BoardState::EXTENDED_BOARD_SIZE + 1,
	    pos - 1,
	    pos + 1,
	    pos + engine::BoardState::EXTENDED_BOARD_SIZE - 1,
	    pos + engine::BoardState::EXTENDED_BOARD_SIZE,
	    pos + engine::BoardState::EXTENDED_BOARD_SIZE + 1,
	};
	PatternHash hash = 0;
	uint32_t offset = 0;
	for (auto neighbor : neighbors)
	{
		hash |= static_cast<unsigned char>(state.board[neighbor]) << offset;
		offset += 2;
	}
	return hash;
}

PatternType matches_pattern(const engine::BoardState& state, uint32_t pos)
{
	return global_pattern_table[get_pattern_hash(state, pos)];
}

} // namespace mcts
} // namespace go