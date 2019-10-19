#ifndef SRC_ENGINE_LIBERTIES_H_
#define SRC_ENGINE_LIBERTIES_H_

#include <stdint.h>

namespace go
{
namespace engine
{
struct BoardState;

// Slowest and most basic liberty counting function that doesn't depend
// on cached data
uint32_t count_liberties(const BoardState& state, uint32_t i, uint32_t j);

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_LIBERTIES_H_