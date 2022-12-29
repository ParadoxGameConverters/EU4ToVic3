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
		const CountryHistoryDate theDate(theStream);
		auto incCharacters = theDate.getCharacters();
		characters.insert(characters.end(), incCharacters.begin(), incCharacters.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
