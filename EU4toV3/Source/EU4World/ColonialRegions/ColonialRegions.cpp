#include "ColonialRegions.h"
#include "ColonialRegion.h"
#include "Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::ColonialRegions::ColonialRegions()
{
	LOG(LogLevel::Info) << "Parsing EU4 colonial regions";

	registerKeys();
	if (!commonItems::DoesFileExist(theConfiguration.getEU4Path() + "/common/colonial_regions/00_colonial_regions.txt"))
		Log(LogLevel::Warning) << "Could not find " << theConfiguration.getEU4Path() + "/common/colonial_regions/00_colonial_regions.txt";
	else
		parseFile(theConfiguration.getEU4Path() + "/common/colonial_regions/00_colonial_regions.txt");

	for (const auto& mod: theConfiguration.getEU4Mods())
		for (const auto& filename: commonItems::GetAllFilesInFolder(mod + "/common/colonial_regions/"))
			parseFile(mod + "/common/colonial_regions/" + filename);

	clearRegisteredKeywords();
}

EU4::ColonialRegions::ColonialRegions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ColonialRegions::registerKeys()
{
	registerRegex(R"(colonial_\w+)", [this](const std::string& regionName, std::istream& theStream) {
		const ColonialRegion newRegion(theStream);
		for (const auto& provinceID: newRegion.getProvinces())
			provinceToColonialRegions.insert(std::make_pair(provinceID, regionName));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> EU4::ColonialRegions::getColonialRegionForProvince(int province) const
{
	if (const auto& mapping = provinceToColonialRegions.find(province); mapping != provinceToColonialRegions.end())
		return mapping->second;
	else
		return std::nullopt;
}
