#ifndef SRC_MCTS_PATTERN3X3_H
#define SRC_MCTS_PATTERN3X3_H

#include "engine/board.h"
#include <array>

namespace go
{
namespace mcts
{

static constexpr size_t PATTERN_TABLE_SIZE = 65536; // 4^8
using Pattern = std::array<engine::Cell, 8>;
using PatternHash = uint16_t;
using PatternType = char;
using PatternTable = std::array<PatternType, PATTERN_TABLE_SIZE>;
static constexpr PatternType NO_PATTERN = -1;

extern PatternTable global_pattern_table;

void load_patterns_file(const std::string& file_name);
PatternHash get_pattern_hash(const Pattern&);
PatternHash get_pattern_hash(const engine::BoardState&, uint32_t);
PatternType matches_pattern(const engine::BoardState&, uint32_t);

} // namespace mcts
} // namespace go

#endif // SRC_MCTS_PATTERN3X3_H