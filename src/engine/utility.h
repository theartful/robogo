#ifndef _ENGINE_UTILITY_H_
#define _ENGINE_UTILITY_H_

#include <algorithm>
#include <array>
#include <type_traits>

#include <stdio.h>
#define DEBUG_PRINT(...)                                                       \
	do                                                                         \
	{                                                                          \
		fprintf(stderr, __VA_ARGS__);                                          \
	} while (0)

namespace go::engine
{

template <size_t N, size_t margin>
static constexpr size_t index(size_t i, size_t j)
{
	return (i + margin) * (N + 2 * margin) + (j + margin);
}

template <size_t N, size_t M, size_t margin = 1, typename T = uint16_t>
struct MarginMapping
{
	static constexpr size_t map(size_t t) { return index_map[t]; }

private:
	static constexpr auto get_index_map()
	{
		std::array<T, (N + 2 * margin) * (M + 2 * margin)> result{};
		for (size_t i = 0; i < N; i++)
			for (size_t j = 0; j < M; j++)
				result[index<N, margin>(i, j)] = index<N, 0>(i, j);
		return result;
	}
	static constexpr auto index_map = get_index_map();
};

template <typename Container, typename Mapping>
class RemappedContainer : public Container
{
	using parent = Container;

public:
	using parent::Container;
	constexpr decltype(auto) operator[](std::size_t pos)
	{
		return parent::operator[](Mapping::map(pos));
	}
	constexpr decltype(auto) operator[](std::size_t pos) const
	{
		return parent::operator[](Mapping::map(pos));
	}
};

template <size_t N, typename Mapping>
class RemappedBitset : public RemappedContainer<std::bitset<N>, Mapping>
{
	using parent = RemappedContainer<std::bitset<N>, Mapping>;

public:
	using parent::RemappedContainer;
	decltype(auto) set(std::size_t pos, bool value = true)
	{
		return parent::set(Mapping::map(pos), value);
	}
	decltype(auto) reset(std::size_t pos)
	{
		return parent::reset(Mapping::map(pos));
	}
	decltype(auto) reset() { return parent::reset(); }
};

template <typename T, size_t N, size_t margin = 1>
using MarginRemapped2DArray_ = std::array<T, 21 * 21>;

template <typename T, size_t N, size_t margin = 1>
using MarginRemapped2DArray =
	RemappedContainer<std::array<T, N * N>, MarginMapping<N, N, margin>>;

template <size_t N, size_t margin = 1>
using MarginRemapped2DBitset =
	RemappedBitset<N * N, MarginMapping<N, N, margin>>;

template <size_t N, size_t margin = 1>
using MarginRemapped2DBitset_ = std::bitset<21 * 21>;

} // namespace go::engine

#endif // SRC_ENGINE_UTILITY_H_
