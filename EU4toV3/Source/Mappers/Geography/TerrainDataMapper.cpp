#include "TerrainDataMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::TerrainDataMapper::TerrainDataMapper()
{
	LOG(LogLevel::Info) << "Loading Terrain Data.";
	registerKeys();
	parseFile("configurables/terrain_data.txt");
	clearRegisteredKeywords();
}

mappers::TerrainDataMapper::TerrainDataMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TerrainDataMapper::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& provID, std::istream& theStream) {
		terrainMap.insert(std::make_pair(stoi(provID), commonItems::singleString(theStream).getString()));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::TerrainDataMapper::getTerrainForID(const int provinceID) const
{
	const auto& terrainItr = terrainMap.find(provinceID);
	if (terrainItr != terrainMap.end())
		return terrainItr->second;
	else
		return std::nullopt;
}