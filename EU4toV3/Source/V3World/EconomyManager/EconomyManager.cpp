#include "EconomyManager.h"
#include "ClayManager/State/SubState.h"
#include "PoliticalManager/Country/Country.h"
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
	// Some global value of CP to spend
	int globalCP = 500000;
	// adjust based on date
	// adjust based on amount of world centralized By..... population?
	int worldPopulation = getWorldPopCount(countries);
	int worldCentralizedPopulation = getCentralizedWorldPopCount();

	// Fronter option 1 the default way. Pop & Eurocentrism
	if (economyType == Configuration::ECONOMY::EuroCentric)
	{
		double totalIndustryScore = 0;
		for (auto country: centralizedCountries)
		{
			int popCount = country->getPopCount();
			country->setIndustryScore(popCount * country->getIndustryFactor() * calculatePopDistanceFactor(popCount, worldCentralizedPopulation));
			totalIndustryScore += country->getIndustryScore();
		}
	}
	// Fronter option 2. Pop & development
	if (economyType == Configuration::ECONOMY::DevBased)
	{
	}
}

int V3::EconomyManager::getCentralizedWorldPopCount() const
{
	return std::accumulate(centralizedCountries.begin(), centralizedCountries.end(), 0, [](int sum, const auto& country) {
		return sum + country->getPopCount();
	});
}

int V3::EconomyManager::getWorldPopCount(const std::map<std::string, std::shared_ptr<Country>>& countries) const
{
	return 0;
}
