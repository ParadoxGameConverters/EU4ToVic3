#include "VNColonialMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::VNColonialMapping::VNColonialMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::VNColonialMapping::registerKeys()
{
	registerKeyword("name", [this](std::istream& theStream) {
		name = commonItems::getString(theStream);
	});
	registerKeyword("owner", [this](std::istream& theStream) {
		owner = commonItems::getString(theStream);
	});
	registerKeyword("decolonize", [this](std::istream& theStream) {
		decolonizeBlocker = commonItems::getString(theStream);
	});
	registerKeyword("key", [this](std::istream& theStream) {
		keyProvince = commonItems::getString(theStream);
	});
	registerKeyword("states", [this](std::istream& theStream) {
		auto theStates = commonItems::getStrings(theStream);
		states.insert(theStates.begin(), theStates.end());
	});
	registerKeyword("subjects", [this](std::istream& theStream) {
		auto theSubjects = commonItems::getStrings(theStream);
		subjects.insert(theSubjects.begin(), theSubjects.end());
	});
	registerKeyword("type", [this](std::istream& theStream) {
		subjectType = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
