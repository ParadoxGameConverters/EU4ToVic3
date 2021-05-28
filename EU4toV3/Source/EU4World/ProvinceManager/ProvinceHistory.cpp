#include "ProvinceHistory.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "DateItems.h"
#include "Log.h"
#include "ParserHelpers.h"

EU4::ProvinceHistory::ProvinceHistory(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ProvinceHistory::registerKeys()
{
	registerKeyword("owner", [this](std::istream& theStream) {
		ownershipHistory.emplace_back(std::make_pair(date("1.1.1"), commonItems::getString(theStream)));
	});
	registerKeyword("culture", [this](std::istream& theStream) {
		const auto culture = commonItems::getString(theStream);
		setStartingCulture(culture);
	});
	registerKeyword("religion", [this](std::istream& theStream) {
		const auto religion = commonItems::getString(theStream);
		setStartingReligion(religion);
	});
	registerSetter("base_tax", originalTax);
	registerSetter("base_production", originalProduction);
	registerSetter("base_manpower", originalManpower);
	registerRegex(R"(\d+\.\d+\.\d+)", [this](const std::string& dateString, std::istream& theStream) {
		auto theDate = date(dateString);
		const DateItems theItems(theStream);
		for (const auto& dateChange: theItems.getDateChanges())
			if (dateChange.changeType == "owner")
				ownershipHistory.emplace_back(std::make_pair(theDate, dateChange.changeValue));
			else if (dateChange.changeType == "culture")
				cultureHistory.emplace_back(std::make_pair(theDate, dateChange.changeValue));
			else if (dateChange.changeType == "religion")
				religionHistory.emplace_back(std::make_pair(theDate, dateChange.changeValue));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void EU4::ProvinceHistory::setStartingCulture(const std::string& culture)
{
	startingCulture = culture;
	if (cultureHistory.empty())
		cultureHistory.emplace_back(std::make_pair(date("1.1.1"), culture));
}

void EU4::ProvinceHistory::setStartingReligion(const std::string& religion)
{
	startingReligion = religion;
	if (religionHistory.empty())
		religionHistory.emplace_back(std::make_pair(date("1.1.1"), religion));
}

void EU4::ProvinceHistory::buildPopRatios(const double assimilationFactor)
{
}
