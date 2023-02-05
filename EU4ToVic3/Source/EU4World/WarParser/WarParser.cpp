#include "WarParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::WarParser::WarParser(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::WarParser::registerKeys()
{
	registerKeyword("name", [this](std::istream& theStream) {
		name = commonItems::getString(theStream);
	});
	registerKeyword("history", [this](std::istream& theStream) {
		details.addDetails(theStream);
	});
	registerKeyword("attackers", [this](std::istream& theStream) {
		attackers = commonItems::getStrings(theStream);
	});
	registerKeyword("defenders", [this](std::istream& theStream) {
		defenders = commonItems::getStrings(theStream);
	});
	registerRegex(R"(take_\w+|defend_\w+|\w*superiority|blockade_\w+)", [this](const std::string& warGoalClass, std::istream& theStream) {
		details.addDetails(theStream);
		details.warGoalClass = warGoalClass;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
