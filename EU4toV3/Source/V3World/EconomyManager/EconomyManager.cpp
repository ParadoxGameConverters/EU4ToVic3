#include "EconomyManager.h"
#include "PoliticalManager/Country/Country.h"
#include <ranges>

auto V3::EconomyManager::selectCentralizedCountries(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	auto selectCentralized = [](std::shared_ptr<Country> country) {
		return country->getProcessedData().type != "decentralized";
	};

	return std::views::values(countries) | std::views::filter(selectCentralized);
}
