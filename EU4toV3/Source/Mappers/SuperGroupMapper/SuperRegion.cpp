#include "SuperRegion.h"
#include "ParserHelpers.h"
#include "Assimilation.h"

mappers::SuperRegion::SuperRegion(std::istream& theStream)
{
	registerRegex("[a-z_]+", [this](const std::string& superRegionName, std::istream& theStream)
		{
			const Assimilation assimilBlock(theStream);
			auto assimilation = assimilBlock.getAssimilation();
			superRegionAssimilationMap.insert(std::make_pair(superRegionName, assimilation));
			superRegionNames.insert(superRegionName);
		});
	registerRegex("[a-zA-Z0-9\\_]+", commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}