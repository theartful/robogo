#include "sgf_parser.h"

#include <cctype>
#include <sstream>

// The implementation of this parser implementation closely follows gnugo's

namespace go::sgf
{

SGFParser::SGFParser(std::istream& in_) : in{in_} { }

std::shared_ptr<SGFNode> SGFParser::parse()
{
	return parse_gametree(nullptr);
}


constexpr int eof = std::char_traits<char>::eof();

static void parse_error(const char *fmt, int arg=0)
{
	fprintf(stderr, fmt, arg);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

static SGFPropertyName prop_to_enum(const char* name)
{
	return static_cast<SGFPropertyName>(prop_to_num(name));
}


void SGFParser::next_token()
{
	do
		lookahead = in.get();
	while (std::isspace(lookahead));
}


void SGFParser::match(int expected)
{
	if (lookahead != expected)
		parse_error("expected: %c", expected);
	else
		next_token();
}

SGFPropertyName SGFParser::parse_property_name()
{
	if (lookahead == eof || !std::isupper(lookahead))
		parse_error("Expected an upper case letter.");

	std::ostringstream ss;
	while (lookahead != EOF && std::isalpha(lookahead))
	{
		if (std::isupper(lookahead))
			ss << lookahead;
		next_token();
	}
	std::string str = ss.str();
	return prop_to_enum(str.c_str());
}


std::string SGFParser::parse_property_value()
{
	std::ostringstream ss;
	match('[');
	while (lookahead != ']' && lookahead != eof)
	{
		if (lookahead == '\\')
		{
			lookahead = in.get();
			if (lookahead == '\r')
			{
				lookahead = in.get();
				if (lookahead == '\n')
					lookahead = in.get();
			}
			else if (lookahead == '\n')
			{
				lookahead = in.get();
				if (lookahead == '\r')
					lookahead = in.get();
			}
			continue;
		}
		ss << lookahead;
		lookahead = in.get();
	}
	match(']');
	return ss.str();
}


void SGFParser::parse_property(const std::shared_ptr<SGFNode>& n)
{
	SGFPropertyName name = parse_property_name();
	do
	{
		std::string value = parse_property_value();
		// FIX: property allowing ranges
		n->props.emplace_back(name, std::move(value));
	}
	while (lookahead == '[');
}


void SGFParser::parse_node(const std::shared_ptr<SGFNode>& n)
{
	match(';');
	while (lookahead != eof && std::isupper(lookahead))
		parse_property(n);
}


std::shared_ptr<SGFNode>
SGFParser::parse_sequence(std::shared_ptr<SGFNode> n)
{
	parse_node(n);
	while (lookahead == ';')
	{
		std::shared_ptr<SGFNode> new_node = std::make_shared<SGFNode>();
		new_node->parent = n;
		n->children.push_back(new_node);
		n = new_node;
		parse_node(n);
	}
	return n;
}


std::shared_ptr<SGFNode>
SGFParser::parse_gametree(const std::shared_ptr<SGFNode>& parent)
{
	while (true)
	{
		if (lookahead == eof)
		{
			parse_error("Empty file?", 0);
			break;
		}
		if (lookahead == '(')
		{
			while (lookahead == '(')
				next_token();
			if (lookahead == ';')
				break;
		}
		next_token();
	}

	auto head = std::make_shared<SGFNode>();
	head->parent = parent;
	std::shared_ptr<SGFNode> last = parse_sequence(head);
	while (lookahead == '(')
		last->children.push_back(parse_gametree(last));

	if (parent)
		match(')');

	return head;
}

}
