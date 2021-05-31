#include "EU4CountryManager.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include <ranges>

void EU4::CountryManager::loadCountries(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CountryManager::registerKeys()
{
	registerKeyword("---", commonItems::ignoreItem);
	registerKeyword("REB", commonItems::ignoreItem);
	registerKeyword("PIR", commonItems::ignoreItem);
	registerKeyword("NAT", commonItems::ignoreItem);
	registerRegex("[A-Z0-9]{3}", [this](const std::string& tag, std::istream& theStream) {
		auto country = std::make_shared<Country>(tag, theStream);
		countries.emplace(tag, country);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::shared_ptr<EU4::Country> EU4::CountryManager::getCountry(const std::string& tag) const
{
	const auto& country = countries.find(tag);
	if (country != countries.end())
		return country->second;
	else
		return nullptr;
}

void EU4::CountryManager::loadUnitTypes(const std::string& EU4Path, const Mods& mods)
{
	unitTypeLoader.loadUnitTypes(EU4Path, mods);
}

void EU4::CountryManager::updateUnitTypes()
{
	for (const auto& country: countries | std::views::values)
		country->updateRegimentTypes(unitTypeLoader);
}
