#include "LawEntry.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V3::LawEntry::LawEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::LawEntry::registerKeys()
{
	registerKeyword("group", [this](std::istream& theStream) {
		law.group = commonItems::getString(theStream);
	});
	registerKeyword("progressiveness", [this](std::istream& theStream) {
		law.progressiveness = commonItems::getInt(theStream);
	});
	registerKeyword("unlocking_technologies", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			law.requiredTechs.emplace(entry);
	});
	registerKeyword("unlocking_laws", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			law.reqiredLaws.emplace(entry);
	});
	registerKeyword("disallowing_laws", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			law.blockingLaws.emplace(entry);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
