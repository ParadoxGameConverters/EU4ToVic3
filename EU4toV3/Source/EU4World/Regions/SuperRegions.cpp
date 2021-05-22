#include "SuperRegions.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::SuperRegions::SuperRegions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::SuperRegions::registerKeys()
{
	registerRegex(R"(\w+_superregion)", [this](const std::string& sregionName, std::istream& theStream) {
		for (const auto& regionName: commonItems::getStrings(theStream))
		{
			if (regionName == "restrict_charter")
				continue;
			superRegionMap[sregionName].insert(regionName);
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

