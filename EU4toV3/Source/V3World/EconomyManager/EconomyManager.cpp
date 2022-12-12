#include "EconomyManager.h"
#include "ClayManager/State/SubState.h"
#include "EU4World/CountryManager/EU4Country.h"
#include "PoliticalManager/Country/Country.h"
#include <cmath>
#include <numeric>
#include <ranges>

void V3::EconomyManager::loadCentralizedCountries(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	auto selectCentralized = [](std::shared_ptr<Country> country) {
		return country->getProcessedData().type != "decentralized";
	};

	for (auto country: std::ranges::filter_view(std::views::values(countries), selectCentralized))
	{
		centralizedCountries.push_back(country);
	}
}

void V3::EconomyManager::assignCountryCPBudgets(Configuration::ECONOMY economyType, const std::map<std::string, std::shared_ptr<Country>>& countries) const
{
	// Some global value of CP to spend, calibrate to Vanilla.
	int globalCP = 500000;
	// adjust based on date
	globalCP *= 1;
	// adjust based on amount of world centralized by population, calibrated to Vanilla
	int worldPopulation = getWorldPopCount(countries);
	int worldCentralizedPopulation = getCentralizedWorldPopCount();
	double globalPopfactor = worldCentralizedPopulation / worldPopulation / .975;

	// Fronter option 1 the default way. Pop & Eurocentrism
	if (economyType == Configuration::ECONOMY::EuroCentric)
	{
		int geoMeanPop = calculateGeoMeanCentralizedPops();

		// while determining individual country's industry score, accumulate total industry score & factor
		double totalIndustryScore = 0;
		double totalIndustryFactor = 0;
		for (auto country: centralizedCountries)
		{
			int popCount = country->getPopCount();
			country->setIndustryScore(popCount * country->getIndustryFactor() * calculatePopDistanceFactor(popCount, geoMeanPop));
			totalIndustryScore += country->getIndustryScore();
			totalIndustryFactor += country->getIndustryFactor();
		}

		// adjust global total by average industry factor compared to baseline
		double globalIndustryFactor = totalIndustryFactor / centralizedCountries.size() / 0.8;
		globalCP *= (globalIndustryFactor + globalPopfactor);

		// distribute each country its budget
		distributeBudget(globalCP, totalIndustryScore);
	}

	/*
	// Fronter option 2. Pop & Tech Group. Like EuroCentrism but blunter
	if (economyType == Configuration::ECONOMY::TechGroup)
	{
		int geoMeanPop = calculateGeoMeanCentralizedPops();

		// while determining individual country's industry score, accumulate total industry score & factor
		double totalIndustryScore = 0;
		double totalTechGroupFactor = 0;
		for (auto country: centralizedCountries)
		{
			// Get country's EU4 techgroup and a config file map to get tech group factor
			double techGroupFactor = country->getSourceCountry()->getTechGroup().size();

			int popCount = country->getPopCount();
			country->setIndustryScore(popCount * techGroupFactor * calculatePopDistanceFactor(popCount, geoMeanPop));
			totalIndustryScore += country->getIndustryScore();
			totalTechGroupFactor += country->getIndustryFactor();
		}

		// adjust global total by average tech group modifier compared to baseline
		double globalTechGroupFactor = totalTechGroupFactor / centralizedCountries.size() / 0.8;
		globalCP *= (totalTechGroupFactor + globalPopfactor);

		// distribute each country it's budget
		distributeBudget(globalCP, totalIndustryScore);
	}

	// Fronter option 3. Pop & development
	if (economyType == Configuration::ECONOMY::DevBased)
	{
		// The more pop you have per dev, the less powerful your development
		// This is loosely assuming Dev = Pop + Economy so Economy = Dev - Pop

		// while determining individual country's industry score, accumulate total industry score & factor
		double totalIndustryScore = 0;
		double totalIndustryFactor = 0;
		for (auto country: centralizedCountries)
		{
			country->setIndustryScore(country->getPopCount() * 1);
			totalIndustryScore += country->getIndustryScore();
			totalIndustryFactor += country->getIndustryFactor();
		}

		globalCP *= globalPopfactor;

		// distribute each country its budget
		distributeBudget(globalCP, totalIndustryScore);
	}
	*/
}

int V3::EconomyManager::getCentralizedWorldPopCount() const
{
	return std::accumulate(centralizedCountries.begin(), centralizedCountries.end(), 0, [](int sum, const auto& country) {
		return sum + country->getPopCount();
	});
}

int V3::EconomyManager::getWorldPopCount(const std::map<std::string, std::shared_ptr<Country>>& theCountries) const
{
	auto countries = std::views::values(theCountries);
	return std::accumulate(countries.begin(), countries.end(), 0, [](int sum, const auto& country) {
		return sum + country->getPopCount();
	});
}

double V3::EconomyManager::calculatePopDistanceFactor(int countryPopulation, double geoMeanPopulation)
{
	if (double pop_percent = geoMeanPopulation / countryPopulation; pop_percent >= 1)
	{
		return log(pop_percent) + 1;
	}
	else
	{
		return log(pop_percent + 1) + 0.7;
	}
}

double V3::EconomyManager::calculateGeoMeanCentralizedPops() const
{
	double sum = 0;

	for (auto country: centralizedCountries)
	{
		sum += log(country->getPopCount());
	}

	sum /= centralizedCountries.size();

	return exp(sum);
}

void V3::EconomyManager::distributeBudget(double globalCP, double totalIndustryScore) const
{
	for (auto country: centralizedCountries)
	{
		country->setCPBudget(static_cast<int>(std::round(globalCP * (country->getIndustryScore() / totalIndustryScore))));
	}
}
