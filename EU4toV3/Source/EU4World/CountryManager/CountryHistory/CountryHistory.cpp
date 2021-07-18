#include "CountryHistory.h"
#include "CommonRegexes.h"
#include "CountryHistoryDate.h"
#include "ParserHelpers.h"

EU4::CountryHistory::CountryHistory(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CountryHistory::registerKeys()
{
	registerSetter("primary_culture", primaryCulture);
	registerSetter("religion", religion);
	registerRegex(R"(\d+\.\d+\.\d+)", [this](const std::string& unused, std::istream& theStream) {
		const CountryHistoryDate theDate(theStream, std::string());
		auto incLeaders = theDate.getLeaders();
		leaders.insert(leaders.end(), incLeaders.begin(), incLeaders.end());
		if (!theDate.getDynasty().empty())
			dynasty = theDate.getDynasty();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
