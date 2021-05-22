#include "BuildingPosition.h"
#include "NavalBase.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::BuildingPosition::BuildingPosition(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::BuildingPosition::registerKeys()
{
	registerKeyword("building_position", [this](const std::string& unused, std::istream& theStream) {
		const NavalBase potentialBase(theStream);
		found = potentialBase.isFound();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
