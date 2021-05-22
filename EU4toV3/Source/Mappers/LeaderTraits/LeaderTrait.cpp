#include "LeaderTrait.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::LeaderTrait::LeaderTrait(std::istream& theStream, std::string theName): name(std::move(theName))
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::LeaderTrait::registerKeys()
{
	registerKeyword("fire", [this](const std::string& unused, std::istream& theStream) {
		fire = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("shock", [this](const std::string& unused, std::istream& theStream) {
		shock = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("maneuver", [this](const std::string& unused, std::istream& theStream) {
		maneuver = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("siege", [this](const std::string& unused, std::istream& theStream) {
		siege = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("other", [this](const std::string& unused, std::istream& theStream) {
		other = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool mappers::LeaderTrait::matches(const int& leaderFire, const int& leaderShock, const int& leaderManeuver, const int& leaderSiege) const
{
	if (leaderFire < fire || leaderShock < shock || leaderManeuver < maneuver || leaderSiege < siege)
		return false;

	// other consists of the sum of all non-required attributes
	if (other > 0)
	{
		auto incOther = 0;
		if (fire == 0)
			incOther += leaderFire;
		if (shock == 0)
			incOther += leaderShock;
		if (maneuver == 0)
			incOther += leaderManeuver;
		if (siege == 0)
			incOther += leaderSiege;

		if (incOther < other)
			return false;
	}
	return true;
}
