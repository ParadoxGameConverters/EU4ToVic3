#include "DefaultMapParser.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <fstream>

void EU4::DefaultMapParser::loadDefaultMap(const commonItems::ModFilesystem& modFS)
{
	// This is a case where mods take priority over definitions. If a mod has defs than we use those and ignore EU4 installation.
	registerKeys();

	const auto& file = modFS.GetActualFileLocation("/map/default.map");
	if (!file)
		throw std::runtime_error("/map/default.map cannot be found!");

	parseFile(*file);
	Log(LogLevel::Info) << "<> " << seas.size() << " seas and " << lakes.size() << " lakes registered.";

	clearRegisteredKeywords();
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
