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
	for (const auto& fileName: modFS.GetAllFilesInFolder("common/cultures"))
	{
		if (fileName.extension() != ".txt")
			continue;

		if (fileName.string().find("99_") != std::string::npos)
			skipProcessing = false;
		else
			skipProcessing = true;

		if (fileName.string().find("blankMod") != std::string::npos)
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
	static const std::vector<std::string> injectReplaceKeywords =
		 {"INJECT:", "REPLACE:", "TRY_INJECT:", "TRY_REPLACE:", "INJECT_OR_CREATE:", "REPLACE_OR_CREATE:"};

	registerRegex(commonItems::catchallRegex, [this](const std::string& cultureNameStr, std::istream& theStream) {
		// If cultureName starts with any of the inject/replace keywords, strip them.
		std::string cultureName = cultureNameStr;
		for (const auto& keyword: injectReplaceKeywords)
		{
			if (cultureName.starts_with(keyword))
			{
				cultureName = cultureName.substr(keyword.size());
				break;
			}
		}

		auto relDef = CultureDefinitionEntry(theStream, skipProcessing, skipExport).getCultureDef();
		relDef.name = cultureName;

		if (!cultureDefinitions.contains(cultureName))
			cultureDefinitions.emplace(cultureName, relDef);
		else
			cultureDefinitions.at(cultureName) = relDef;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
