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

void EU4::CountryManager::linkProvincesToCountries(const ProvinceManager& provinceManager)
{
	const auto& provinces = provinceManager.getAllProvinces();

	for (const auto& province: provinces | std::views::values)
	{
		// add provinces to all coreowners.
		for (const auto& core: province->getCores())
			if (const auto& coreOwner = countries.find(core); coreOwner != countries.end())
				coreOwner->second->addCore(province);

		// add to actual owner
		if (province->getOwnerTag().empty())
			continue;
		if (const auto& owner = countries.find(province->getOwnerTag()); owner != countries.end())
			owner->second->addProvince(province);
	}
}

void EU4::CountryManager::setHREAndEmperors(const std::string& HREmperor, const std::string& celestialEmperor, const ProvinceManager& provinceManager)
{
	for (const auto& country: countries | std::views::values)
	{
		// set HRE stuff
		if (country->getCapital() != 0)
		{
			const auto& capital = provinceManager.getProvince(country->getCapital());
			if (capital && capital->inHre())
				country->setInHRE(true);
		}
		if (country->getTag() == HREmperor)
			country->setEmperor(true);

		// not-HRE stuff
		if (country->getTag() == celestialEmperor)
			country->setCelestialEmperor(true);
	}
}
