#include "ProvinceHistory.h"
#include "Configuration.h"
#include "DateItems.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::ProvinceHistory::ProvinceHistory(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (!startingCulture.empty() && (cultureHistory.empty() || cultureHistory.begin()->first != theConfiguration.getStartEU4Date()))
	{
		cultureHistory.insert(cultureHistory.begin(), std::make_pair(theConfiguration.getStartEU4Date(), startingCulture));
	}
	if (!startingReligion.empty() && (religionHistory.empty() || religionHistory.begin()->first != theConfiguration.getStartEU4Date()))
	{
		religionHistory.insert(religionHistory.begin(), std::make_pair(theConfiguration.getStartEU4Date(), startingReligion));
	}
}

void EU4::ProvinceHistory::registerKeys()
{
	registerKeyword("owner", [this](const std::string& unused, std::istream& theStream) {
		ownershipHistory.emplace_back(std::make_pair(theConfiguration.getStartEU4Date(), commonItems::getString(theStream)));
	});
	registerSetter("culture", startingCulture);
	registerSetter("religion", startingReligion);
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

std::optional<date> EU4::ProvinceHistory::getFirstOwnedDate() const
{
	if (!ownershipHistory.empty())
		return ownershipHistory[0].first;
	else
		return std::nullopt;
}

bool EU4::ProvinceHistory::hasOriginalCulture() const
{
	if (cultureHistory.size() > 1 && cultureHistory[0].second != cultureHistory[cultureHistory.size() - 1].second)
		return false;
	else
		return true;
}

bool EU4::ProvinceHistory::wasColonized() const
{
	if (!ownershipHistory.empty() && ownershipHistory[0].first != theConfiguration.getStartEU4Date() &&
		 ownershipHistory[0].first != theConfiguration.getFirstEU4Date())
		return !hasOriginalCulture();
	else
		return false;
}

