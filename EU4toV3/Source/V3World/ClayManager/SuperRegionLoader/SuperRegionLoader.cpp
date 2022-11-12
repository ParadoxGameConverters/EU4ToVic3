#include "SuperRegionLoader.h"
#include "CommonFunctions.h"
#include "OSCompatibilityLayer.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

void V3::SuperRegionLoader::loadSuperRegions(const std::string& v3Path)
{
	for (const auto& fileName: commonItems::GetAllFilesInFolder(v3Path + "/common/strategic_regions/"))
	{
		if (getExtension(fileName) != "txt")
			continue;
		std::ifstream superRegionStream(fs::u8path(v3Path + "/common/strategic_regions/" + fileName));
		if (!superRegionStream.is_open())
			throw std::runtime_error("Could not open " + v3Path + "/common/strategic_regions/" + fileName + " !");

		const auto superRegion = std::make_shared<SuperRegion>();
		const auto superRegionName = trimExtension(fileName);
		superRegion->initializeSuperRegion(superRegionStream);
		superRegion->setName(superRegionName);
		superRegions.emplace(superRegionName, superRegion);
	}
}
