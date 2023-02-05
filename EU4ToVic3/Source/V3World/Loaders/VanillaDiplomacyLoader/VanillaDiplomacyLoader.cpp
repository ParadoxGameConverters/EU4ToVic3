#include "VanillaDiplomacyLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "VanillaDiplomacyEntries.h"

void V3::VanillaDiplomacyLoader::loadVanillaDiplomacy(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/history/diplomacy/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::VanillaDiplomacyLoader::registerKeys()
{
	registerKeyword("DIPLOMACY", [this](std::istream& theStream) {
		const auto entries = VanillaDiplomacyEntries(theStream);
		const auto& theAgreements = entries.getAgreementEntries();
		const auto& theRelations = entries.getRelationEntries();
		const auto& theTruces = entries.getTruceEntries();
		agreementEntries.insert(agreementEntries.end(), theAgreements.begin(), theAgreements.end());
		relationEntries.insert(relationEntries.end(), theRelations.begin(), theRelations.end());
		truceEntries.insert(truceEntries.end(), theTruces.begin(), theTruces.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
