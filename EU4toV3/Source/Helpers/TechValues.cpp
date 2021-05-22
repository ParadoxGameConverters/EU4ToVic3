#include "TechValues.h"
#include "../EU4World/Country/EU4Country.h"
#include "../V3World/Country/Country.h"
#include <algorithm>

helpers::TechValues::TechValues(const std::map<std::string, std::shared_ptr<V2::Country>>& countries)
{
	gatherScores(countries);
	if (armyScores.empty()) return; // Well crap. Play dead and hope they go away.
	calculateSteps();
}

void helpers::TechValues::gatherScores(const std::map<std::string, std::shared_ptr<V2::Country>>& countries)
{
	for (const auto& countryItr: countries)
	{
		const auto& country = countryItr.second;
		if (!isValidCountryForTechConversion(*country)) continue;
		
		armyScores.emplace_back(getCountryArmyTech(*country->getSourceCountry()));
		navyScores.emplace_back(getCountryNavyTech(*country->getSourceCountry()));
		commerceScores.emplace_back(getCountryCommerceTech(*country->getSourceCountry()));
		cultureScores.emplace_back(getCountryCultureTech(*country->getSourceCountry()));
		industryScores.emplace_back(getCountryIndustryTech(*country->getSourceCountry()));
	}
}

void helpers::TechValues::calculateSteps()
{
	// Drop all repeated scores, not using sets because we need indexes
	std::sort(armyScores.begin(), armyScores.end());
	armyScores.erase(std::unique(armyScores.begin(), armyScores.end()), armyScores.end());
	std::sort(navyScores.begin(), navyScores.end());
	navyScores.erase(std::unique(navyScores.begin(), navyScores.end()), navyScores.end());
	std::sort(commerceScores.begin(), commerceScores.end());
	commerceScores.erase(std::unique(commerceScores.begin(), commerceScores.end()), commerceScores.end());
	std::sort(cultureScores.begin(), cultureScores.end());
	cultureScores.erase(std::unique(cultureScores.begin(), cultureScores.end()), cultureScores.end());
	std::sort(industryScores.begin(), industryScores.end());
	industryScores.erase(std::unique(industryScores.begin(), industryScores.end()), industryScores.end());

	// For a given set of scores, how much does each rank matter? Scale on -1 to 1.
	// For a single civilized nation (roman empire or smlr.) it will get 1 in everything. As a consequence
	// no country will ever have -1 in anything, but this is not an issue in practice.
	armyStep = 2.0 / armyScores.size();
	navyStep = 2.0 / navyScores.size();
	commerceStep = 2.0 / commerceScores.size();
	cultureStep = 2.0 / cultureScores.size();
	industryStep = 2.0 / industryScores.size();	

}

bool helpers::TechValues::isValidCountryForTechConversion(const V2::Country& country)
{
	return country.isCivilized() && !country.getProvinces().empty() && country.getSourceCountry();
}

double helpers::TechValues::getNormalizedArmyTech(const EU4::Country& country) const
{
	const auto score = getCountryArmyTech(country);
	const auto it = std::find(armyScores.begin(), armyScores.end(), score);
	const auto index = std::distance(armyScores.begin(), it);
	return (static_cast<double>(index) + 1) * armyStep - 1;
}

double helpers::TechValues::getNormalizedNavyTech(const EU4::Country& country) const
{
	const auto score = getCountryNavyTech(country);
	const auto it = std::find(navyScores.begin(), navyScores.end(), score);
	const auto index = std::distance(navyScores.begin(), it);
	return (static_cast<double>(index) + 1) * navyStep - 1;
}

double helpers::TechValues::getNormalizedCommerceTech(const EU4::Country& country) const
{
	const auto score = getCountryCommerceTech(country);
	const auto it = std::find(commerceScores.begin(), commerceScores.end(), score);
	const auto index = std::distance(commerceScores.begin(), it);
	return (static_cast<double>(index) + 1) * commerceStep - 1;
}

double helpers::TechValues::getNormalizedCultureTech(const EU4::Country& country) const
{
	const auto score = getCountryCultureTech(country);
	const auto it = std::find(cultureScores.begin(), cultureScores.end(), score);
	const auto index = std::distance(cultureScores.begin(), it);
	return (static_cast<double>(index) + 1) * cultureStep - 1;
}

double helpers::TechValues::getNormalizedIndustryTech(const EU4::Country& country) const
{
	const auto score = getCountryIndustryTech(country);
	const auto it = std::find(industryScores.begin(), industryScores.end(), score);
	const auto index = std::distance(industryScores.begin(), it);
	return (static_cast<double>(index) + 1) * industryStep - 1;
}

double helpers::TechValues::getCountryArmyTech(const EU4::Country& country)
{
	return (country.getMilTech() + country.getAdmTech()) * (1 + country.getArmy() / 10);
}

double helpers::TechValues::getCountryNavyTech(const EU4::Country& country)
{
	return (country.getMilTech() + country.getDipTech()) * (1 + country.getNavy() / 10);
}

double helpers::TechValues::getCountryCommerceTech(const EU4::Country& country)
{
	return (country.getAdmTech() + country.getDipTech()) * (1 + country.getCommerce() / 10);
}

double helpers::TechValues::getCountryCultureTech(const EU4::Country& country)
{
	return country.getDipTech() * (1 + country.getCulture() / 10);
}

double helpers::TechValues::getCountryIndustryTech(const EU4::Country& country)
{
	return (country.getAdmTech() + country.getDipTech() + country.getMilTech()) * ( 1.0 + country.getIndustry() / 10.0);
}

