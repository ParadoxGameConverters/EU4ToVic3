#include "TitleMapping.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::TitleMapping::TitleMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TitleMapping::registerKeys()
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
		transform(name.begin(), name.end(), name.begin(), ::tolower);
	});
	registerKeyword("title", [this](const std::string& unused, std::istream& theStream) {
		title = commonItems::singleString(theStream).getString();
	});
	registerKeyword("region", [this](const std::string& unused, std::istream& theStream) {
		region = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool mappers::TitleMapping::hasIslamicRegion() const
{
	return region == "e_persia" || region == "e_arabia";
}

bool mappers::TitleMapping::hasIndianRegion() const
{
	return region == "e_rajastan" || region == "e_bengal" || region == "e_deccan";
}
