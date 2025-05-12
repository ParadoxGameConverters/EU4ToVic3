#include "TraitDefinitionLoader.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "TraitDefinitionEntry.h"

void mappers::TraitDefinitionLoader::loadDefinitions(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading discrimination trait definitions.";
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/discrimination_traits/"))
	{
		if (getExtension(fileName) != "txt")
			continue;

		parseFile(fileName);
	}
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << traitDefinitions.size() << " definitions loaded.";
}

void mappers::TraitDefinitionLoader::loadDefinitions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TraitDefinitionLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& traitName, std::istream& theStream) {
		auto traitDef = TraitDefinitionEntry(theStream).getTraitDef();
		traitDef.name = traitName;
		traitDefinitions.emplace(traitName, traitDef);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
