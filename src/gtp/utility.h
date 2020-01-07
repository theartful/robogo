#ifndef GTP_UTILITY_H_
#define GTP_UTILITY_H_

#include <algorithm>
#include <array>
#include <variant>

namespace go
{
namespace gtp
{

char get_column_char(uint32_t col);
uint32_t get_column_idx(char col_char);

// very dirty implementation of expected class
template <typename E>
struct unexpected
{
	E e;
	template <typename T>
	unexpected(T&& e_) : e{std::forward<T>(e_)}
	{
	}
};

template <typename T>
unexpected(T&& e_)->unexpected<std::decay_t<T>>;

namespace details
{
template <typename T>
using escape_void =
    std::conditional_t<std::is_same_v<T, void>, std::monostate, T>;
}

template <typename T, typename E = std::string>
class expected : public std::variant<details::escape_void<T>, unexpected<E>>
{
	using parent = std::variant<details::escape_void<T>, unexpected<E>>;

public:
	using value_type = details::escape_void<T>;
	using error_type = E;

	using std::variant<details::escape_void<T>, unexpected<E>>::variant;

	bool has_value() const
	{
		return parent::index() == 0;
	}

	const value_type& value() const
	{
		return std::get<0>(*this);
	}

	const error_type& error() const
	{
		return std::get<1>(*this).e;
	}
};

template <typename T>
struct is_value_or_error
{
	struct yes;
	struct no;

	template <typename C>
	static yes
	test(decltype(&C::has_value), decltype(&C::value), decltype(&C::error));
	template <typename C>
	static no test(...);

public:
	static constexpr bool value =
	    std::is_same_v<decltype(test<T>(0, 0, 0)), yes>;
};

template <typename T>
static constexpr bool is_value_or_error_v = is_value_or_error<T>::value;

namespace details
{
template <class Callable, class Container, std::size_t... I>
constexpr decltype(auto)
apply_impl(Callable&& func, Container&& t, std::index_sequence<I...>)
{
	return std::invoke(std::forward<Callable>(func), t[I]...);
}
} // namespace details

template <size_t N, class Callable, class Container>
constexpr decltype(auto) apply(Callable&& func, Container&& t)
{
	return details::apply_impl(
	    std::forward<Callable>(func), std::forward<Container>(t),
	    std::make_index_sequence<N>{});
}

} // namespace gtp
} // namespace go

#endif // GTP_UTILITY_H_