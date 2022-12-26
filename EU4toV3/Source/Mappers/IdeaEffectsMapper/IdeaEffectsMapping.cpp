#include "IdeaEffectsMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::IdeaEffectsMapping::IdeaEffectsMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::IdeaEffectsMapping::registerKeys()
{
	registerKeyword("literacy", [this](std::istream& theStream) {
		literacy = commonItems::getInt(theStream);
	});
	registerKeyword("adm", [this](std::istream& theStream) {
		adm = commonItems::getInt(theStream);
	});
	registerKeyword("dip", [this](std::istream& theStream) {
		dip = commonItems::getInt(theStream);
	});
	registerKeyword("mil", [this](std::istream& theStream) {
		mil = commonItems::getInt(theStream);
	});
	registerKeyword("ig", [this](std::istream& theStream) {
		boostedInterestGroups.emplace(commonItems::getString(theStream));
	});
	registerKeyword("noig", [this](std::istream& theStream) {
		suppressedInterestGroups.emplace(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
