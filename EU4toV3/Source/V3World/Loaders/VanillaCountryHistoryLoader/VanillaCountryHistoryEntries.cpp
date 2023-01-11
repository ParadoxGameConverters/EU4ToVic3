#include "VanillaCountryHistoryEntries.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V3::VanillaCountryHistoryEntries::VanillaCountryHistoryEntries(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::VanillaCountryHistoryEntries::registerKeys()
{
	registerRegex(R"([c,C]:\w+)", [this](const std::string& sourceTag, std::istream& theStream) {
		activeTag = sourceTag.substr(2, sourceTag.size());
		entryParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	entryParser.registerRegex(commonItems::catchallRegex, [this](const std::string& elementName, std::istream& theStream) {
		const auto elementValue = commonItems::stringOfItem(theStream);
		countryHistoryElements[activeTag].emplace_back(elementName + " " + elementValue.getString());
	});
}
