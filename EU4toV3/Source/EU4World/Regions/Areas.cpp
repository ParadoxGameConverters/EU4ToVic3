#include "Areas.h"
#include "Area.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::Areas::Areas(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::Areas::registerKeys()
{
	registerRegex(commonItems::stringRegex, [this](const std::string& areaName, std::istream& theStream) {
		const Area newArea(theStream);
		theAreas.emplace(areaName, newArea.getProvinces());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::set<int>> EU4::Areas::getProvincesInArea(const std::string& area) const
{
	const auto& areaItr(theAreas.find(area));
	if (areaItr != theAreas.end())
		return areaItr->second;
	return std::nullopt;
}