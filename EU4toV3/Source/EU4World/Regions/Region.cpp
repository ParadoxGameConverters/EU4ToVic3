#include "Region.h"
#include "Areas.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::Region::Region(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::Region::registerKeys()
{
	registerKeyword("areas", [this](std::istream& theStream) {
		for (const auto& areaName: commonItems::getStrings(theStream))
			areaNames.insert(areaName);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

EU4::Region::Region(const std::set<int>& provinces)
{
	areaProvinces["dummyArea"] = provinces;
	areaNames.insert("dummyArea");
}

bool EU4::Region::regionContainsProvince(int province) const
{
	for (const auto& area: areaProvinces)
		if (area.second.count(province))
			return true;
	return false;
}

bool EU4::Region::areaContainsProvince(const std::string& areaName, const int province) const
{
	// Support for old installations without areas:
	const auto& areaItr = areaProvinces.find("dummyArea");
	if (areaItr != areaProvinces.end())
		if (areaItr->second.count(province))
			return true;

	const auto& areaItr2 = areaProvinces.find(areaName);
	if (areaItr2 == areaProvinces.end())
		return false;
	if (areaItr2->second.count(province))
		return true;
	return false;
}

void EU4::Region::addProvinces(const Areas& areas)
{
	for (const auto& areaName: areaNames)
	{
		const auto& newProvinces = areas.getProvincesInArea(areaName);
		if (newProvinces)
			areaProvinces[areaName] = *newProvinces;
	}
}