#include "TerrainModifierLoader.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include <Log.h>

void V3::TerrainModifierLoader::loadTerrainModifiers(const std::string& filePath)
{
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
}

void V3::TerrainModifierLoader::loadTerrainModifiers(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::TerrainModifierLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& terrain, std::istream& theStream) {
		stateTerrainModifiers[terrain] = 0;

		for (const auto& [type, multiplier]: commonItems::assignments(theStream).getAssignments())
		{
			try
			{
				if (type == "priority")
				{
					stateTerrainModifiers[terrain] = std::stod(multiplier);
				}
				else
				{
					buildingTerrainModifiers[terrain][type] = std::stod(multiplier);
				}
			}
			catch (const std::exception& e)
			{
				Log(LogLevel::Error) << "Failed read " << type << " under " << terrain << " modifier: " << e.what();
			}
		}
	});
}