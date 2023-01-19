#include "TechSetupMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::TechSetupMapping::TechSetupMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TechSetupMapping::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& techName, std::istream& theStream) {
		techScores.emplace(techName, commonItems::getInt(theStream));
	});
}
