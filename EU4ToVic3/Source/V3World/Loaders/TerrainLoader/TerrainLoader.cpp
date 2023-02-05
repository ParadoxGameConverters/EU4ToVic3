#include "TerrainLoader.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "StringUtils.h"

void V3::TerrainLoader::loadTerrains(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	parseFile(*modFS.GetActualFileLocation("/map_data/province_terrains.txt"));
	clearRegisteredKeywords();
}

void V3::TerrainLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& provinceName, std::istream& theStream) {
		const auto theTerrain = commonItems::getString(theStream);
		auto theProvinceName = commonItems::remQuotes(provinceName);
		std::transform(theProvinceName.begin(), theProvinceName.end(), theProvinceName.begin(), ::toupper);
		if (theProvinceName.starts_with("X") && theProvinceName.size() == 7)
			theProvinceName = "x" + theProvinceName.substr(1, theProvinceName.length() - 1);
		else
			Log(LogLevel::Warning) << "Encountered terrain for province " << theProvinceName << " in unknown format!";
		provincialTerrains.emplace(theProvinceName, theTerrain);
	});
}
