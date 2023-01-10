#include "VanillaDiplomacyEntries.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V3::VanillaDiplomacyEntries::VanillaDiplomacyEntries(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::VanillaDiplomacyEntries::registerKeys()
{
	registerRegex(R"([c,C]:\w+)", [this](const std::string& sourceTag, std::istream& theStream) {
		activeTag = sourceTag.substr(2, sourceTag.size());
		entryParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	entryParser.registerKeyword("create_diplomatic_pact", [this](std::istream& theStream) {
		agreementEntries.emplace_back(VanillaAgreementEntry(theStream, activeTag));
	});
	entryParser.registerKeyword("set_relations", [this](std::istream& theStream) {
		relationEntries.emplace_back(VanillaRelationEntry(theStream, activeTag));
	});
	entryParser.registerKeyword("create_truce", [this](std::istream& theStream) {
		truceEntries.emplace_back(VanillaTruceEntry(theStream, activeTag));
	});
	entryParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
