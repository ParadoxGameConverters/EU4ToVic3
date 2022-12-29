#include "CountryHistoryDate.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::CountryHistoryDate::CountryHistoryDate(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CountryHistoryDate::registerKeys()
{
	registerKeyword("leader", [this](std::istream& theStream) {
		Leader leader;
		leader.parseLeader(theStream);
		characters.push_back(leader.getCharacter());
	});
	registerRegex("monarch|monarch_heir|heir|queen", [this](const std::string& rulerType, std::istream& theStream) {
		Leader leader;
		leader.parseRuler(theStream);
		auto character = leader.getCharacter();
		if (rulerType == "monarch" || rulerType == "monarch_heir")
			character.ruler = true;
		else if (rulerType == "heir")
			character.heir = true;
		else
			character.consort = true;
		characters.push_back(character);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
