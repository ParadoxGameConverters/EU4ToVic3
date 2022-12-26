#include "LawMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::LawMapping::LawMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::LawMapping::registerKeys()
{
	registerKeyword("forms", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			forms.emplace(entry);
	});
	registerKeyword("ideas", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			ideas.emplace(entry);
	});
	registerKeyword("reforms", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			reforms.emplace(entry);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
