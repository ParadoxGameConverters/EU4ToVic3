#include "AfricaResetMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::AfricaResetMapper::AfricaResetMapper()
{
	LOG(LogLevel::Info) << "Parsing african resettable regions.";
	registerKeys();
	parseFile("configurables/africa_reset.txt");
	clearRegisteredKeywords();
}

mappers::AfricaResetMapper::AfricaResetMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::AfricaResetMapper::registerKeys()
{
	registerKeyword("reset_regions", [this](const std::string& unused, std::istream& theStream) {
		auto theRegions = commonItems::stringList(theStream).getStrings();
		resettableRegions = std::set(theRegions.begin(), theRegions.end());
	});
	registerKeyword("reset_below_westernization", [this](const std::string& unused, std::istream& theStream) {
		reset_below_westernization = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
