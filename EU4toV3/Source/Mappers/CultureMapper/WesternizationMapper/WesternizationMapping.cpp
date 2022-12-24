#include "WesternizationMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::WesternizationMapping::WesternizationMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::WesternizationMapping::registerKeys()
{
	registerKeyword("westernization", [this](std::istream& theStream) {
		westernization = commonItems::getInt(theStream);
	});
	registerKeyword("literacy", [this](std::istream& theStream) {
		literacy = commonItems::getInt(theStream);
	});
	registerKeyword("industry", [this](std::istream& theStream) {
		industry = commonItems::getInt(theStream);
	});
	registerKeyword("traits", [this](std::istream& theStream) {
		const auto& traitList = commonItems::getStrings(theStream);
		traits.insert(traitList.begin(), traitList.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
