#include "PoliticalManager.h"
#include "CountryDefinitionLoader/CountryDefinitionLoader.h"
#include "Log.h"
#include <ranges>

void V3::PoliticalManager::initializeVanillaCountries(const std::string& V3Path)
{
	Log(LogLevel::Info) << "-> Loading Vanilla Countries.";

	CountryDefinitionLoader definitionLoader;
	definitionLoader.loadCommonCountries(V3Path);
	countries = definitionLoader.getCountries();

	Log(LogLevel::Info) << "<> " << countries.size() << " vanilla countries loaded.";
}

void V3::PoliticalManager::importEU4Countries(const std::map<std::string, std::shared_ptr<EU4::Country>>& eu4Countries)
{
	Log(LogLevel::Info) << "-> Moving over EU4 Countries.";

	for (const auto& eu4country: eu4Countries | std::views::values)
	{
		auto v3Tag = countryMapper->assignV3TagToEU4Country(eu4country);

		std::shared_ptr<Country> country;
		if (countries.contains(v3Tag))
		{
			country = countries.at(v3Tag);
		}
		else
		{
			country = std::make_shared<Country>();
			country->setTag(v3Tag);
			countries.emplace(v3Tag, country);
		}

		country->setSourceCountry(eu4country);
		country->initializeFromEU4Country();
	}

	Log(LogLevel::Info) << "<> Vic3 now has " << countries.size() << " known countries.";
}

void V3::PoliticalManager::importVanillaCountries()
{
	// TODO: TESTS
	for (const auto& country: countries | std::views::values)
		if (country->getVanillaData() && !country->getSourceCountry()) // this is a vic3-only country.
			country->copyVanillaData();
}
