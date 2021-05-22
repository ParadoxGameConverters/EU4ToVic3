#include "EU4War.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::War::War(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::War::registerKeys()
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::getString(theStream);
	});
	registerKeyword("history", [this](const std::string& unused, std::istream& theStream) {
		details.addDetails(theStream);
	});
	registerKeyword("attackers", [this](const std::string& unused, std::istream& theStream) {
		attackers = commonItems::getStrings(theStream);
	});
	registerKeyword("defenders", [this](const std::string& unused, std::istream& theStream) {
		defenders = commonItems::getStrings(theStream);
	});
	registerRegex(R"(take_\w+|defend_\w+|\w*superiority|blockade_\w+)", [this](const std::string& warGoalClass, std::istream& theStream) {
		details.addDetails(theStream);
		details.warGoalClass = warGoalClass;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
