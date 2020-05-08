#ifndef _SGF_SGF_PARSER_H_
#define _SGF_SGF_PARSER_H_

#include "engine/game.h"

#include <memory>
#include <vector>

namespace go::sgf
{

static constexpr uint16_t prop_to_num(const char* name)
{
	return name[0] | (name[1] << 8);
}

enum class SGFPropertyName : uint16_t
{
	Annotation = prop_to_num("AN"),
	Application = prop_to_num("AP"),
	Comment = prop_to_num("C"),
	GameComment = prop_to_num("GC"),
	AddBlack = prop_to_num("AB"),
	AddWhite = prop_to_num("AW"),
	BlackMove = prop_to_num("B"),
	WhiteMove = prop_to_num("W"),
	BlackRank = prop_to_num("BR"),
	WhiteRank = prop_to_num("WR"),
	BlackTeam = prop_to_num("BT"),
	WhiteTeam = prop_to_num("WT"),
	Date = prop_to_num("DT"),
	Event = prop_to_num("EV"),
	FileFormat = prop_to_num("FF"),
	GameType = prop_to_num("GM"),
	GameName = prop_to_num("GN"),
	Handicap = prop_to_num("HA"),
	BlackName = prop_to_num("PB"),
	WhiteName = prop_to_num("PW"),
	Result = prop_to_num("RE"),
	Size = prop_to_num("SZ"),
	User = prop_to_num("US"),
	Rules = prop_to_num("RU"),
	Komi = prop_to_num("KM"),
	StartColor = prop_to_num("PL"),
	Place = prop_to_num("PC"),
	Source = prop_to_num("SO"),
};

struct SGFProperty
{
	SGFPropertyName name;
	std::string value;
	SGFProperty(SGFPropertyName name_, std::string value_) :
		name{name_}, value{value_}
	{
	}
};

struct SGFNode
{
	std::vector<SGFProperty> props;
	std::shared_ptr<SGFNode> parent;
	std::vector<std::shared_ptr<SGFNode>> children;
};

void execute_sgf(
	const std::shared_ptr<SGFNode>&, engine::GameState&, engine::Rules&,
	int32_t = -1);

class SGFParser
{
public:
	SGFParser(std::istream& in_);
	std::shared_ptr<SGFNode> parse();

private:
	void next_token();
	void match(int expected);
	SGFPropertyName parse_property_name();
	std::string parse_property_value();
	void parse_property(const std::shared_ptr<SGFNode>&);
	void parse_node(const std::shared_ptr<SGFNode>&);
	std::shared_ptr<SGFNode> parse_gametree(const std::shared_ptr<SGFNode>&);
	std::shared_ptr<SGFNode> parse_sequence(std::shared_ptr<SGFNode>);

private:
	std::istream& in;
	int lookahead;
};

} // namespace go::sgf

#endif // _SGF_SGF_PARSER_H_
