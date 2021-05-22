#include "CulturalUnion.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "ParserHelpers.h"

mappers::CulturalUnion::CulturalUnion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (theConfiguration.isHpmEnabled() && !hpm.empty())
	{
		tags = hpm;
	}
}

void mappers::CulturalUnion::registerKeys()
{
	registerKeyword("culture", [this](const std::string& unused, std::istream& theStream) {
		culture = commonItems::singleString(theStream).getString();
	});
	registerKeyword("tag", [this](const std::string& unused, std::istream& theStream) {
		tags.insert(commonItems::singleString(theStream).getString());
	});
	registerKeyword("hpm", [this](const std::string& unused, std::istream& theStream) {
		hpm.insert(commonItems::singleString(theStream).getString());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
