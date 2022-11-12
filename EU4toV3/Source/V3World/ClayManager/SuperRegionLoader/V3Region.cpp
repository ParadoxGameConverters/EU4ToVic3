#include "V3Region.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

void V3::Region::initializeRegion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::Region::registerKeys()
{
	registerKeyword("states", [this](std::istream& theStream) {
		const auto& stateNames = commonItems::getStrings(theStream);
		for (const auto& stateName: stateNames)
			states.emplace(stateName, nullptr);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
