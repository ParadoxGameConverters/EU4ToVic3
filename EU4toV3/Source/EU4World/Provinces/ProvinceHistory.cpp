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

	if (theConfiguration.getConvertAll())
	{
		// Drop conversion history if we're converting everyone by config.
		if (!cultureHistory.empty())
		{
			auto lastCulture = cultureHistory.back();
			lastCulture.first = theConfiguration.getStartEU4Date();
			cultureHistory.clear();
			cultureHistory.emplace_back(lastCulture);
		}
		if (!religionHistory.empty())
		{
			auto lastReligion = religionHistory.back();
			lastReligion.first = theConfiguration.getStartEU4Date();
			religionHistory.clear();
			religionHistory.emplace_back(lastReligion);
		}
	}

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

void EU4::ProvinceHistory::buildPopRatios(const double assimilationFactor)
{
	// Don't build pop ratios for empty strings.
	if (cultureHistory.empty() || religionHistory.empty())
		return;

	auto endDate = theConfiguration.getLastEU4Date();
	if (endDate > HARD_ENDING_DATE || !endDate.isSet())
		endDate = HARD_ENDING_DATE;

	std::string startCulture;
	auto cultureEvent = cultureHistory.begin();
	if (cultureEvent != cultureHistory.end())
	{
		startCulture = cultureEvent->second;
		++cultureEvent;
	}

	std::string startReligion;
	auto religionEvent = religionHistory.begin();
	if (religionEvent != religionHistory.end())
	{
		startReligion = religionEvent->second;
		++religionEvent;
	}

	PopRatio currentRatio(startCulture, startReligion);
	date cultureEventDate;
	date religionEventDate;
	auto lastLoopDate = theConfiguration.getStartEU4Date();
	while (cultureEvent != cultureHistory.end() || religionEvent != religionHistory.end())
	{
		if (cultureEvent == cultureHistory.end())
			cultureEventDate = FUTURE_DATE;
		else
			cultureEventDate = cultureEvent->first;

		if (religionEvent == religionHistory.end())
			religionEventDate = FUTURE_DATE;
		else
			religionEventDate = religionEvent->first;

		if (cultureEventDate < religionEventDate)
		{
			decayPopRatios(lastLoopDate, cultureEventDate, currentRatio, assimilationFactor);
			popRatios.push_back(currentRatio);
			for (auto& itr: popRatios)
				itr.convertFrom();

			currentRatio.convertToCulture(cultureEvent->second);
			lastLoopDate = cultureEventDate;
			++cultureEvent;
		}
		else if (cultureEventDate == religionEventDate)
		{
			// culture and religion change on the same day;
			decayPopRatios(lastLoopDate, cultureEventDate, currentRatio, assimilationFactor);
			popRatios.push_back(currentRatio);
			for (auto& itr: popRatios)
				itr.convertFrom();

			currentRatio.convertTo(cultureEvent->second, religionEvent->second);
			lastLoopDate = cultureEventDate;
			++cultureEvent;
			++religionEvent;
		}
		else if (religionEventDate < cultureEventDate)
		{
			decayPopRatios(lastLoopDate, religionEventDate, currentRatio, assimilationFactor);
			popRatios.push_back(currentRatio);
			for (auto& itr: popRatios)
				itr.convertFrom();

			currentRatio.convertToReligion(religionEvent->second);
			lastLoopDate = religionEventDate;
			++religionEvent;
		}
	}
	decayPopRatios(lastLoopDate, endDate, currentRatio, assimilationFactor);

	if (!currentRatio.getCulture().empty() || !currentRatio.getReligion().empty())
		popRatios.push_back(currentRatio);
}

void EU4::ProvinceHistory::decayPopRatios(const date& oldDate, const date& newDate, PopRatio& currentPop, const double assimilationFactor)
{
	// no decay needed for initial state
	if (oldDate == theConfiguration.getStartEU4Date())
		return;

	const auto diffInYears = newDate.diffInYears(oldDate);
	for (auto& popRatio: popRatios)
		popRatio.decay(diffInYears, assimilationFactor);

	currentPop.increase(diffInYears, assimilationFactor);
}

void EU4::ProvinceHistory::updatePopRatioCulture(const std::string& oldCultureName, const std::string& neoCultureName, const std::string& superRegion)
{
	for (auto& popRatio: popRatios)
		if (popRatio.getCulture() == oldCultureName)
			popRatio.setNeoCulture(neoCultureName, superRegion);
}
