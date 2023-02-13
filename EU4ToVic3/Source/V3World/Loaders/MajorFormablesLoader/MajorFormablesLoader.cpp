#include "MajorFormablesLoader.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "MajorFormablesEntry.h"

void V3::MajorFormablesLoader::loadMajorFormables(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading Major formables.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << majorFormables.size() << " major formables.";
}

void V3::MajorFormablesLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& tag, std::istream& theStream) {
		majorFormables.emplace(tag, MajorFormablesEntry(theStream));
	});
}
