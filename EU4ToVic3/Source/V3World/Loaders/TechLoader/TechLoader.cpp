#include "TechLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "TechEntry.h"

void V3::TechLoader::loadTechs(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading Techs.";
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/technology/technologies"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << techs.size() << " techs.";
}

void V3::TechLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& techName, std::istream& theStream) {
		techs.emplace(techName, TechEntry(theStream).getTech());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
