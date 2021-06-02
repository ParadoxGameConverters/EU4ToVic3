#include "ColonialRegionLoader.h"
#include "ColonialRegion.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"
#include <ranges>

void EU4::ColonialRegionLoader::loadColonialRegions(const std::string& EU4Path, const Mods& mods)
{	
	registerKeys();
	
	if (!commonItems::DoesFileExist(EU4Path + "/common/colonial_regions/00_colonial_regions.txt"))
		Log(LogLevel::Warning) << "Could not find " << EU4Path + "/common/colonial_regions/00_colonial_regions.txt";
	else
		parseFile(EU4Path + "/common/colonial_regions/00_colonial_regions.txt");

	for (const auto& mod: mods | std::views::values)
		for (const auto& filename: commonItems::GetAllFilesInFolder(mod + "/common/colonial_regions/"))
			parseFile(mod + "/common/colonial_regions/" + filename);

	clearRegisteredKeywords();

	Log(LogLevel::Info) << "<> " << provinceToColonialRegions.size() << " provinces linked to colonial regions.";
}

void EU4::ColonialRegionLoader::loadColonialRegions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ColonialRegionLoader::registerKeys()
{
	registerRegex(R"(colonial_\w+)", [this](const std::string& regionName, std::istream& theStream) {
		const ColonialRegion newRegion(theStream);
		for (const auto& provinceID: newRegion.getProvinces())
			provinceToColonialRegions.emplace(provinceID, regionName);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> EU4::ColonialRegionLoader::getColonialRegionForProvince(int province) const
{
	if (const auto& mapping = provinceToColonialRegions.find(province); mapping != provinceToColonialRegions.end())
		return mapping->second;
	else
		return std::nullopt;
}
