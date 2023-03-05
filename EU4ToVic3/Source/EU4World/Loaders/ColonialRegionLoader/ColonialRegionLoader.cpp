#include "ColonialRegionLoader.h"
#include "ColonialRegion.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <ranges>

void EU4::ColonialRegionLoader::loadColonialRegions(const commonItems::ModFilesystem& modFS)
{
	registerKeys();

	for (const auto& file: modFS.GetAllFilesInFolder("/common/colonial_regions/"))
	{
		if (getExtension(file) != "txt")
			continue;
		parseFile(file);
	}

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
