#include "CountryHistoryDate.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::CountryHistoryDate::CountryHistoryDate(std::istream& theStream, const std::string& leaderClass)
{
	registerKeys(leaderClass);
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CountryHistoryDate::registerKeys(const std::string& leaderClass)
{
	registerKeyword("leader", [this](const std::string& unused, std::istream& theStream) {
		const Leader newLeader(theStream);
		leaders.push_back(newLeader);
	});
	registerRegex("monarch|heir|queen", [this](const std::string& theLeaderClass, std::istream& theStream) {
		const CountryHistoryDate lookForLeader(theStream, theLeaderClass);
		const auto& incLeaders = lookForLeader.getLeaders();
		leaders.insert(leaders.end(), incLeaders.begin(), incLeaders.end());
		if (!lookForLeader.dynasty.empty())
			dynasty = lookForLeader.dynasty;
	});
	registerKeyword("dynasty", [this, leaderClass](std::istream& theStream) {
		const auto& dynastyString = commonItems::getString(theStream);
		if (leaderClass == "monarch")
			dynasty = dynastyString;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
