#include "CultureDefinitionLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "CultureDefinitionEntry.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::CultureDefinitionLoader::loadDefinitions(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading culture definitions.";
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/cultures/"))
	{
		if (getExtension(fileName) != "txt")
			continue;

		if (fileName.find("99_") != std::string::npos)
			skipProcessing = false;
		else
			skipProcessing = true;

		if (fileName.find("blankMod") != std::string::npos)
			skipExport = true;
		else
			skipExport = false;

		parseFile(fileName);
	}
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << cultureDefinitions.size() << " definitions loaded.";
}

void mappers::CultureDefinitionLoader::loadDefinitions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureDefinitionLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& cultureName, std::istream& theStream) {
		auto relDef = CultureDefinitionEntry(theStream, skipProcessing, skipExport).getCultureDef();
		relDef.name = cultureName;

		if (!cultureDefinitions.contains(cultureName))
			cultureDefinitions.emplace(cultureName, relDef);
		else
			cultureDefinitions.at(cultureName) = relDef;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
