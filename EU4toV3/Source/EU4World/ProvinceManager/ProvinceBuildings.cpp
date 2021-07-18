#include "ProvinceBuildings.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::ProvinceBuildings::ProvinceBuildings(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ProvinceBuildings::registerKeys()
{
	registerRegex(commonItems::stringRegex, [this](const std::string& building, std::istream& theStream) {
		commonItems::ignoreItem(building, theStream);
		buildings.insert(building);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
