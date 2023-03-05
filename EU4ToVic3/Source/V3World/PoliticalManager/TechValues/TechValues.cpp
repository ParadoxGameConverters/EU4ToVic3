#include "TechValues.h"
#include "CountryManager/EU4Country.h"
#include "PoliticalManager/Country/Country.h"
#include <algorithm>
#include <ranges>

V3::TechValues::TechValues(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	gatherScores(countries);
	if (productionScores.empty())
		return; // Well crap. Play dead and hope they go away.
	calculateOrders();
}

void V3::TechValues::gatherScores(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	for (const auto& [tag, country]: countries)
	{
		if (!isValidCountryForTechConversion(*country))
			continue;
		auto bonus = 0;
		if (country->getProcessedData().westernized && country->getSourceCountry()->isGP())
			bonus = 2.0;

		productionScores.emplace(tag, getCountryProductionTech(*country) + bonus);
		militaryScores.emplace(tag, getCountryMilitaryTech(*country) + bonus);
		societyScores.emplace(tag, getCountrySocietyTech(*country) + bonus);
	}
}

void V3::TechValues::calculateOrders()
{
	productionOrder = sortMap(productionScores);
	militaryOrder = sortMap(militaryScores);
	societyOrder = sortMap(societyScores);
}

std::vector<std::string> V3::TechValues::sortMap(const std::map<std::string, double>& theMap)
{
	std::vector<std::string> sorted;

	std::vector<std::pair<std::string, double>> pairs;
	for (const auto& theElement: theMap)
		pairs.emplace_back(theElement);

	std::sort(pairs.begin(), pairs.end(), [=](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
		return a.second < b.second;
	});

	for (const auto& tag: pairs | std::views::keys)
		sorted.emplace_back(tag);

	return sorted;
}

bool V3::TechValues::isValidCountryForTechConversion(const Country& country)
{
	return !country.getSubStates().empty() && country.getSourceCountry();
}

double V3::TechValues::getProductionTechPercentile(const std::string& tag) const
{
	if (!productionScores.contains(tag))
		return 0;

	const auto it = std::find(productionOrder.begin(), productionOrder.end(), tag);
	const auto index = std::distance(productionOrder.begin(), it);
	return (static_cast<double>(index) + 1) / static_cast<double>(productionOrder.size()) * 100;
}

double V3::TechValues::getMilitaryTechPercentile(const std::string& tag) const
{
	if (!militaryScores.contains(tag))
		return 0;

	const auto it = std::find(militaryOrder.begin(), militaryOrder.end(), tag);
	const auto index = std::distance(militaryOrder.begin(), it);
	return (static_cast<double>(index) + 1) / static_cast<double>(militaryOrder.size()) * 100;
}

double V3::TechValues::getSocietyTechPercentile(const std::string& tag) const
{
	if (!societyScores.contains(tag))
		return 0;

	const auto it = std::find(societyOrder.begin(), societyOrder.end(), tag);
	const auto index = std::distance(societyOrder.begin(), it);
	return (static_cast<double>(index) + 1) / static_cast<double>(societyOrder.size()) * 100;
}

/* These three functions do heavy tech shaping according to civLevel. However civLevel was a *function* of
 * incoming tech and low-tech countries already have low civlevel!
 *
 * This means a lagging country will be twice penalized for its lack of tech, but an eurocentric-shaped one (eg.
 * to 60%) will now have tech score reduced only to that 60%. That means we're in fact *counting on* EU4 importing
 * all countries with mostly same levels of tech, and double-penalizing the stragglers.
 *
 * However, we're only reducing the tech score for purpose of *rank* and median values will still apply, and grant
 * everyone properly distributed tech *levels* (it will just heavily favor westernized countries).
 */

double V3::TechValues::getCountryProductionTech(const Country& country)
{
	auto totalDip = country.getSourceCountry()->getDipTech() + country.getProcessedData().ideaEffect.dip;
	totalDip += country.getSourceCountry()->getScore() / 100'000;
	return totalDip * country.getProcessedData().civLevel / 100;
}

double V3::TechValues::getCountryMilitaryTech(const Country& country)
{
	auto totalMil = country.getSourceCountry()->getMilTech() + country.getProcessedData().ideaEffect.mil;
	totalMil += country.getSourceCountry()->getScore() / 100'000;
	return totalMil * country.getProcessedData().civLevel / 100;
}

double V3::TechValues::getCountrySocietyTech(const Country& country)
{
	auto totalAdm = country.getSourceCountry()->getAdmTech() + country.getProcessedData().ideaEffect.adm;
	totalAdm += country.getSourceCountry()->getScore() / 100'000;
	return totalAdm * country.getProcessedData().civLevel / 100;
}
