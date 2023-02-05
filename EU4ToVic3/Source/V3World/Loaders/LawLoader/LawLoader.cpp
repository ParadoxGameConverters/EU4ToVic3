#include "LawLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "LawEntry.h"
#include "Log.h"
#include "ParserHelpers.h"

void V3::LawLoader::loadLaws(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading Laws.";
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/laws/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << laws.size() << " laws.";
}

void V3::LawLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& lawName, std::istream& theStream) {
		laws.emplace(lawName, LawEntry(theStream).getLaw());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
