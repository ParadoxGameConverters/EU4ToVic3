#include "TechEntry.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V3::TechEntry::TechEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::TechEntry::registerKeys()
{
	registerKeyword("modifier", [this](std::istream& theStream) {
		modifierParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	modifierParser.registerKeyword("state_infrastructure_from_population_add", [this](std::istream& theStream) {
		tech.infrastructureMult = commonItems::getDouble(theStream);
	});
	modifierParser.registerKeyword("state_infrastructure_from_population_max_add", [this](std::istream& theStream) {
		tech.infrastructureMax = commonItems::getInt(theStream);
	});
	modifierParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
