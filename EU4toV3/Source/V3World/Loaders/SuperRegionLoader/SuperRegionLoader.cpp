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
		if (getExtension(fileName) != "txt")
			continue;
		std::ifstream superRegionStream(fs::u8path(fileName));
		if (!superRegionStream.is_open())
			throw std::runtime_error("Could not open " + fileName + " !");

		const auto superRegion = std::make_shared<SuperRegion>();
		const auto superRegionName = trimPath(trimExtension(fileName));
		superRegion->initializeSuperRegion(superRegionStream);
		superRegion->setName(superRegionName);
		superRegions.emplace(superRegionName, superRegion);
	}
}
