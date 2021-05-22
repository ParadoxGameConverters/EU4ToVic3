#include "ClimateMapper.h"
#include "Configuration.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ClimateMapper::ClimateMapper()
{
	LOG(LogLevel::Info) << "Loading climates.";
	registerKeys();
	parseFile(theConfiguration.getVic2Path() + "/map/climate.txt");
	clearRegisteredKeywords();
}

mappers::ClimateMapper::ClimateMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ClimateMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& climate, std::istream& theStream) {
		if (seenSoFar.contains(climate))
		{
			climateMap.insert(std::make_pair(climate, commonItems::intList(theStream).getInts()));
		}
		else
		{
			commonItems::ignoreItem(climate, theStream);
			seenSoFar.insert(climate);
		}
	});
}
