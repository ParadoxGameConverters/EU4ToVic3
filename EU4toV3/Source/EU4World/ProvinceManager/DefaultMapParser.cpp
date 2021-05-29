#include "DefaultMapParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "OSCompatibilityLayer.h"
#include <fstream>

void EU4::DefaultMapParser::loadDefaultMap(const std::string& EU4Path, const Mods& mods)
{
	// This is a case where mods take priority over definitions. If a mod has defs than we use those and ignore EU4 installation.
	registerKeys();
	
	for (const auto& [modName, modPath]: mods)
		if (commonItems::DoesFileExist(modPath + "/map/default.map"))
		{
			std::ifstream definitionsFile(modPath + "/map/default.map");
			parseStream(definitionsFile);
			definitionsFile.close();
			Log(LogLevel::Info) << "<> " << seas.size() << " seas and " << lakes.size() << " lakes registered from:" << modName;
			clearRegisteredKeywords();
			return;
		}

	if (!commonItems::DoesFileExist(EU4Path + "/map/default.map"))
		throw std::runtime_error(EU4Path + "/map/default.map cannot be found!");

	std::ifstream definitionsFile(EU4Path + "/map/default.map");
	parseStream(definitionsFile);
	definitionsFile.close();
	clearRegisteredKeywords();

	Log(LogLevel::Info) << "<> " << seas.size() << " seas and " << lakes.size() << " lakes registered.";
}

void EU4::DefaultMapParser::loadDefaultMap(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::DefaultMapParser::registerKeys()
{
	registerKeyword("sea_starts", [this](std::istream& theStream) {
		auto seaList = commonItems::getInts(theStream);
		seas.insert(seaList.begin(), seaList.end());
	});
	registerKeyword("only_used_for_random", [this](std::istream& theStream) {
		auto rnwList = commonItems::getInts(theStream);
		randoms.insert(rnwList.begin(), rnwList.end());
	});
	registerKeyword("lakes", [this](std::istream& theStream) {
		auto lakeList = commonItems::getInts(theStream);
		lakes.insert(lakeList.begin(), lakeList.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
