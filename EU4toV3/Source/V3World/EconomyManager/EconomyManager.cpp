#include "EconomyManager.h"
#include "PoliticalManager/Country/Country.h"
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

void V3::EconomyManager::assignCountryCPBudgets(const Configuration::ECONOMY& economySwitch) const
{
	// Fronter option 1 the default way. Pop & Eurocentrism
	if (true)
	{
		
	}
	// Fronter option 2 the mp map way. Pop & development
}
