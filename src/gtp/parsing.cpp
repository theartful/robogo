#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "engine/board.h"
#include "gtp/gtp.h"
#include "gtp/utility.h"

namespace go::gtp
{

static constexpr auto get_column_char_arr()
{
	return std::array{
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K',
		'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'
	};
}

char get_column_char(uint32_t col)
{
	auto column_char_arr = get_column_char_arr();
	if (static_cast<size_t>(col) < column_char_arr.size())
		return column_char_arr[col];
	else
		return '?';
}

uint32_t get_column_idx(char col_char)
{
	auto column_char_arr = get_column_char_arr();
	auto it = std::find(
		column_char_arr.begin(), column_char_arr.end(),
		static_cast<char>(std::toupper(static_cast<unsigned char>(col_char))));

	return static_cast<size_t>(std::distance(column_char_arr.begin(), it));
}

static std::ostream& operator<<(std::ostream& stream, const Vertex& vertex)
{
	return stream << get_column_char(vertex.col) << vertex.row;
}

static std::ostream& operator<<(std::ostream& stream, const Color& color)
{
	switch(color)
	{
	case Color::Black:
		return stream << "black";
	case Color::White:
		return stream << "white";
	case Color::Empty:
		return stream << "empty";
	default:
		return stream;
	}
}

static std::ostream&
operator<<(std::ostream& stream, const std::monostate& /*unused*/)
{
	return stream;
}

template <typename ValueOrError>
static std::enable_if_t<is_value_or_error_v<ValueOrError>, std::ostream&>
operator<<(std::ostream& stream, const ValueOrError& ve)
{
	if (ve.has_value())
		stream << ve.value();
	else
		stream << ve.error();
	return stream;
}

template <typename T>
static std::ostream&
operator<<(std::ostream& stream, const std::vector<T>& vec)
{
	if (!vec.empty())
	{
		stream << vec.front();
		for (auto it = vec.begin() + 1; it != vec.end(); ++it)
			stream << ' ' << *it;
	}
	return stream;
}

// assumes null terminated strings
template <typename T>
static std::optional<T> from_str(std::string_view str)
{
	return T{str};
}

template <>
std::optional<float> from_str<float>(std::string_view str)
{
	char* end;
	float val = strtof(str.data(), &end);
	if (*end != '\0')
		return {};
	return val;
}

template <>
std::optional<int32_t> from_str<int32_t>(std::string_view str)
{
	char* end;
	long val = strtol(str.data(), &end, 10);
	if (*end != '\0')
		return {};
	return static_cast<uint32_t>(val);
}

template <>
std::optional<uint32_t> from_str<uint32_t>(std::string_view str)
{
	auto val = from_str<int32_t>(str);
	if (!val || val.value() < 0)
		return {};
	return static_cast<uint32_t>(val.value());
}

template <>
std::optional<Vertex> from_str<Vertex>(std::string_view str)
{
	if (str.size() > 3 || str.size() < 2)
		return {};

	uint32_t col = get_column_idx(str[0]);

	std::string_view row_str{str.data() + 1, str.size() - 1};
	if (auto row = from_str<uint32_t>(row_str); row)
		return Vertex{row.value() - 1, col};
	else
		return {};
}

template <>
std::optional<Color> from_str<Color>(std::string_view str)
{
	if (str == "b" || str == "black")
		return Color::Black;
	else if (str == "w" || str == "white")
		return Color::White;
	else
		return {};
}

namespace details
{
template <typename R, typename... Args, typename Callable>
static GTPFunction to_gtp_func_impl(Callable&& func)
{
	return [func](const GTPCommand& command) -> GTPCommandResult {
		using namespace std;
		constexpr size_t num_func_args = sizeof...(Args);

		if (command.args.size() != num_func_args)
			return GTPCommandResult{"invalid number of arguments", true};

		// convert argument strings to their type in the function
		auto args = apply<num_func_args>(
			[](auto&&... ts) { return make_tuple(from_str<Args>(ts)...); },
			command.args);

		// check if any conversion failed
		if (apply([](auto&&... ts) { return (!ts.has_value() || ...); }, args))
			return GTPCommandResult{"error parsing arguments", true};

		auto call_func = [&]() {
			auto call_func_ = [&]() {
				return apply(
					[&](auto&&... ts) { return invoke(func, ts.value()...); },
					args);
			};
			if constexpr (is_same_v<R, void>)
				return call_func_(), monostate{};
			else
				return call_func_();
		};
		auto error_occurred = [](auto&& val) -> bool {
			if constexpr (is_value_or_error_v<R>)
				return !val.has_value();
			else
				return false;
		};

		std::ostringstream oss;
		oss << std::boolalpha;
		auto result = call_func();
		oss << result;
		return GTPCommandResult{oss.str(), error_occurred(result)};
	};
}
} // namespace details

template <typename R, typename... Args>
GTPFunction GTPController::to_gtp_func(R (GTPController::*func)(Args...))
{
	return details::to_gtp_func_impl<R, Args...>(std::bind_front(func, this));
}

static GTPCommand parse(std::string_view str)
{
	std::vector<std::string_view> tokens;

	auto word_begin = str.begin();
	while (word_begin < str.end())
	{
		const auto word_end = std::find_if(
				word_begin, str.end(), [](char c) { return c == '\0'; });

		if (word_end != word_begin)
			tokens.emplace_back(
				word_begin,
				static_cast<size_t>(std::distance(word_begin, word_end)));

		word_begin = word_end + 1;
	}

	if (tokens.empty())
		return GTPCommand{};

	// check id
	bool has_id = !tokens[0].empty() &&
		std::all_of(tokens[0].begin(), tokens[0].end(), [](unsigned char c) {
			return std::isdigit(c);
		});

	std::string_view id;
	auto it = tokens.begin();
	if (has_id)
		id = *(it++);

	std::string_view command;
	if (it != tokens.end())
		command = *(it++);

	return GTPCommand(id, command, {it, tokens.end()});
}

static void preprocess(std::string& request)
{
	// replace delimiters by null and convert to lower case
	std::transform(
		request.begin(), request.end(), request.begin(),
		[](unsigned char c) -> char {
			constexpr std::array delims = {' ', '\t'};
			if (std::find(delims.begin(), delims.end(), c) != delims.end())
				return '\0';
			else
				return std::tolower(c);
	});

	// remove comments
	auto it = std::find(request.begin(), request.end(), '#');
	if (it != request.end())
		request.resize(static_cast<size_t>(std::distance(request.begin(), it)));
}

void GTPController::main_loop(std::ostream& out, std::istream& in)
{
	std::string request;
	while (!quit_flag)
	{
		std::getline(in, request);
		if (request.empty())
			continue;
		preprocess(request);

		GTPCommand gtp_command = parse(request);
		auto it = function_map.find(gtp_command.command);
		if (it == function_map.end())
		{
			out << '?' << gtp_command.id << ' ';
			out << "unknown command: " << gtp_command.command;
			out << std::endl << std::endl;
		}
		else
		{
			auto result = it->second(gtp_command);
			if (result.error)
				out << '?';
			else
				out << '=';
			out << gtp_command.id << ' ';
			out << result.output << std::endl << std::endl;
		}
	}
}

GTPController::GTPController() : game{}, quit_flag{false}
{
	function_map = {
		{"list_commands", to_gtp_func(&GTPController::list_commands)},
		{"protocol_version", to_gtp_func(&GTPController::protocol_version)},
		{"name", to_gtp_func(&GTPController::name)},
		{"version", to_gtp_func(&GTPController::version)},
		{"known_command", to_gtp_func(&GTPController::known_command)},
		{"showboard", to_gtp_func(&GTPController::showboard)},
		{"play", to_gtp_func(&GTPController::play)},
		{"clear_board", to_gtp_func(&GTPController::clear_board)},
		{"komi", to_gtp_func(&GTPController::komi)},
		{"boardsize", to_gtp_func(&GTPController::boardsize)},
		{"quit", to_gtp_func(&GTPController::quit)},
		// {"rg_showboard", to_gtp_func(&GTPController::rg_showboard)},
		{"countlib", to_gtp_func(&GTPController::countlib)},
		{"findlib", to_gtp_func(&GTPController::findlib)},
		{"is_legal", to_gtp_func(&GTPController::is_legal)},
		{"is_suicide", to_gtp_func(&GTPController::is_suicide)},
		{"color", to_gtp_func(&GTPController::color)},
		{"captures", to_gtp_func(&GTPController::captures)},
	};
	for (auto&& pair : function_map)
		known_commands.push_back(pair.first);
}

} // namespace go::gtp
