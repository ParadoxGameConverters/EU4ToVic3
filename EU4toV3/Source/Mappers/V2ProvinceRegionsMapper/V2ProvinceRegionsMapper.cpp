#include "V2ProvinceRegionsMapper.h"
#include "Configuration.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::V2ProvinceRegionsMapper::V2ProvinceRegionsMapper()
{
	registerKeys();
	if (commonItems::DoesFileExist("./blankMod/output/map/region.txt"))
		parseFile("./blankMod/output/map/region.txt");
	else
		parseFile(theConfiguration.getVic2Path() + "/map/region.txt");
	clearRegisteredKeywords();
}

mappers::V2ProvinceRegionsMapper::V2ProvinceRegionsMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::V2ProvinceRegionsMapper::registerKeys()
{
	registerRegex(R"(\w\w\w_\d+)", [this](const std::string& regionName, std::istream& theStream) {
		for (auto provinceID: commonItems::intList(theStream).getInts())
			provinceRegions.insert(std::pair(provinceID, regionName));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::V2ProvinceRegionsMapper::getRegionForProvince(const int province) const
{
	if (const auto& provinceItr = provinceRegions.find(province); provinceItr != provinceRegions.end())
		return provinceItr->second;
	else
		return std::nullopt;
}
