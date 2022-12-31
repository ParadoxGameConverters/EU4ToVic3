#include "CultureDefinitionLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "CultureDefinitionEntry.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::CultureDefinitionLoader::loadDefinitions(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading culture refinitions.";
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/cultures/"))
	{
		if (fileName.contains("99_"))
			skipExport = false;
		else
			skipExport = true;
		if (getExtension(fileName) == "txt")
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
		auto relDef = CultureDefinitionEntry(theStream, skipExport).getCultureDef();
		relDef.name = cultureName;
		cultureDefinitions.emplace(cultureName, relDef);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
