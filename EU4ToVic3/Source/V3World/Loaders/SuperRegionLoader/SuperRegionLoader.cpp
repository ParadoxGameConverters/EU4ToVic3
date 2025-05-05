#include "SuperRegionLoader.h"
#include "CommonFunctions.h"
#include "V3SuperRegion.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

void V3::SuperRegionLoader::loadSuperRegions(const commonItems::ModFilesystem& modFS)
{
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/strategic_regions/"))
	{
		if (fileName.extension() != ".txt")
			continue;
		std::ifstream superRegionStream(fileName);
		if (!superRegionStream.is_open())
			throw std::runtime_error("Could not open " + fileName.string() + " !");

		const auto superRegion = std::make_shared<SuperRegion>();
		const auto superRegionName = fileName.stem();
		superRegion->initializeSuperRegion(superRegionStream);
		superRegion->setName(superRegionName.string());
		superRegions.emplace(superRegionName.string(), superRegion);
	}
}
