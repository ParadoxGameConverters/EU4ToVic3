#include "CountryHistory.h"
#include "CountryHistoryDate.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::CountryHistory::CountryHistory(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CountryHistory::registerKeys()
{
	registerRegex(R"(\d+\.\d+\.\d+)", [this](const std::string& unused, std::istream& theStream) {
		const CountryHistoryDate theDate(theStream, std::string());
		auto incLeaders = theDate.getLeaders();
		leaders.insert(leaders.end(), incLeaders.begin(), incLeaders.end());
		if (!theDate.getDynasty().empty())
			dynasty = theDate.getDynasty();
	});
	registerKeyword("primary_culture", [this](const std::string& unused, std::istream& theStream) {
		primaryCulture = commonItems::singleString(theStream).getString();
	});
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		religion = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
