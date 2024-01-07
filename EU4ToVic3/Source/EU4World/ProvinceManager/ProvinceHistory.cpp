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

void EU4::ProvinceHistory::purgeHistories()
{
	if (!cultureHistory.empty())
	{
		auto lastCulture = cultureHistory.back();
		lastCulture.first = date("1.1.1");
		cultureHistory.clear();
		cultureHistory.emplace_back(lastCulture);
	}
	if (!religionHistory.empty())
	{
		auto lastReligion = religionHistory.back();
		lastReligion.first = date("1.1.1");
		religionHistory.clear();
		religionHistory.emplace_back(lastReligion);
	}
}

void EU4::ProvinceHistory::buildPopRatios(const double assimilationFactor, const DatingData& datingData)
{
	// Don't build pop ratios for empty provinces.
	if (cultureHistory.empty() || religionHistory.empty())
		return;

	auto endDate = datingData.lastEU4Date;
	if (endDate > datingData.hardEndingDate || !endDate.isSet())
		endDate = datingData.hardEndingDate;

	// position iterators at beginning of each queue + 1
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

	// This is the popRatio we're working on. It'll get modified for every entry and copied into popRatios.
	// Idea is that for every new conversion we "make room" by cutting down and decaying *other* popRatios,
	// while growing current one by the number of years that pass between events.

	PopRatio currentRatio(startCulture, startReligion);
	date cultureEventDate;
	date religionEventDate;

	// Sometimes the game queues many repeated events that do the same thing ("set religion to catholic" etc).
	// We have to monitor if we've seen these changes before.
	std::string lastCulture;
	std::string lastReligion;

	// We start not by first date but by EU4 game start. We don't care what happened before.
	auto lastLoopDate = datingData.startEU4Date;
	const auto futureDate = date("9999.1.1"); // this is a future date we set to queues that we don't want to touch any more.

	while (cultureEvent != cultureHistory.end() || religionEvent != religionHistory.end())
	{
		if (cultureEvent == cultureHistory.end())
			cultureEventDate = futureDate;
		else
			cultureEventDate = cultureEvent->first;

		if (religionEvent == religionHistory.end())
			religionEventDate = futureDate;
		else
			religionEventDate = religionEvent->first;

		if (cultureEventDate < religionEventDate)
		{
			if (lastCulture == cultureEvent->second)
			{
				// skip this event entirely, pretend it never happened.
				++cultureEvent;
				continue;
			}

			// Make room for the new pie chart slice and grow the current one
			decayPopRatios(lastLoopDate, cultureEventDate, currentRatio, assimilationFactor);
			// shove in the new slice
			popRatios.push_back(currentRatio);
			// and halve the populations of middle and upper class in every one of them
			// (see tests for explanation)
			for (auto& itr: popRatios)
				itr.convertFrom();

			currentRatio.convertToCulture(cultureEvent->second);
			lastCulture = cultureEvent->second;
			lastLoopDate = cultureEventDate;
			++cultureEvent;
		}
		else if (cultureEventDate == religionEventDate)
		{
			if (lastCulture == cultureEvent->second && lastReligion == religionEvent->second)
			{
				// skip this event entirely, pretend it never happened. One of these *may* be at .end() so we're careful.
				if (cultureEvent != cultureHistory.end())
					++cultureEvent;
				if (religionEvent != religionHistory.end())
					++religionEvent;
				continue;
			}

			// culture and religion change on the same day;
			decayPopRatios(lastLoopDate, cultureEventDate, currentRatio, assimilationFactor);
			popRatios.push_back(currentRatio);
			for (auto& itr: popRatios)
				itr.convertFrom();

			currentRatio.convertTo(cultureEvent->second, religionEvent->second);
			lastCulture = cultureEvent->second;
			lastReligion = religionEvent->second;
			lastLoopDate = cultureEventDate;
			++cultureEvent;
			++religionEvent;
		}
		else if (religionEventDate < cultureEventDate)
		{
			if (lastReligion == religionEvent->second)
			{
				// skip this event entirely, pretend it never happened.
				++religionEvent;
				continue;
			}

			decayPopRatios(lastLoopDate, religionEventDate, currentRatio, assimilationFactor);
			popRatios.push_back(currentRatio);
			for (auto& itr: popRatios)
				itr.convertFrom();

			currentRatio.convertToReligion(religionEvent->second);
			lastReligion = religionEvent->second;
			lastLoopDate = religionEventDate;
			++religionEvent;
		}
	}
	// Finally, fast forward the ratios to the last event date, wrapping up the loop.
	decayPopRatios(lastLoopDate, endDate, currentRatio, assimilationFactor);

	// And shove in the current state of the endgame popRatio.
	if (!currentRatio.getCulture().empty() || !currentRatio.getReligion().empty())
		popRatios.push_back(currentRatio);

	// Now, for provinces that have been victims of tribal migrations they will have many identical ratios. We need to merge them.
	std::vector<PopRatio> mergedRatios;

	auto ratioItr = popRatios.begin();
	while (ratioItr != popRatios.end())
	{
		if (ratioItr->isSpent())
		{
			++ratioItr;
			continue;
		}

		auto otherRatioItr = ratioItr + 1;
		while (otherRatioItr != popRatios.end())
		{
			if (!otherRatioItr->isSpent() && *otherRatioItr == *ratioItr)
			{
				ratioItr->mergeRatio(*otherRatioItr);
				otherRatioItr->markSpent();
			}
			++otherRatioItr;
		}
		mergedRatios.emplace_back(*ratioItr);
		++ratioItr;
	}

	popRatios.swap(mergedRatios);
}

void EU4::ProvinceHistory::decayPopRatios(const date& oldDate, const date& newDate, PopRatio& currentPop, const double assimilationFactor)
{
	if (oldDate == newDate)
		return;

	const auto diffInYears = newDate.diffInYears(oldDate);
	for (auto& popRatio: popRatios)
		popRatio.decay(diffInYears, assimilationFactor);

	currentPop.increase(diffInYears, assimilationFactor);
}

void EU4::ProvinceHistory::markNeoCulture(const std::string& culture)
{
	for (auto& popRatio: popRatios)
		if (popRatio.getCulture() == culture)
			popRatio.markNeoCulture();
}
