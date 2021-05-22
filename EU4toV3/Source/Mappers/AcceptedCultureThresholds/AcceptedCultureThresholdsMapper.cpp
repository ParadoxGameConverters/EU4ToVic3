#include "AcceptedCultureThresholdsMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::AcceptedCultureThresholdsMapper::AcceptedCultureThresholdsMapper()
{
	LOG(LogLevel::Info) << "Parsing accepted culture thresholds.";
	registerKeys();
	parseFile("configurables/accepted_culture_thresholds.txt");
	clearRegisteredKeywords();
}

mappers::AcceptedCultureThresholdsMapper::AcceptedCultureThresholdsMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::AcceptedCultureThresholdsMapper::registerKeys()
{
	registerKeyword("same_group", [this](const std::string& unused, std::istream& theStream) {
		sameGroup = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("foreign_group", [this](const std::string& unused, std::istream& theStream) {
		foreignGroup = commonItems::singleDouble(theStream).getDouble();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
