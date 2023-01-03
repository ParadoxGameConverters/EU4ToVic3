#include "EU4CountryManager.h"
#include "CommonRegexes.h"
#include "Log.h"
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

void EU4::CountryManager::loadCommonCountries(const std::string& EU4Path, const Mods& mods)
{
	commonCountryLoader.loadCommonCountries(EU4Path, mods);
}

void EU4::CountryManager::loadLocalizations(const std::string& EU4Path, const Mods& mods)
{
	localizationLoader.loadLocalizations(EU4Path, mods);
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

	// also record capital information.
	for (const auto& [tag, country]: countries)
	{
		if (country->getCapital() != 0 && provinces.contains(country->getCapital()))
		{
			provinces.at(country->getCapital())->addCapital(tag);
		}
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

void EU4::CountryManager::setRevolutionTarget(const std::string& revolutionTarget)
{
	if (!revolutionTarget.empty())
	{
		const auto& country = countries.find(revolutionTarget);
		if (country != countries.end())
		{
			country->second->setRevolutionary(true);
			Log(LogLevel::Info) << "\t^^^ Revolution Lives!";
			return;
		}
	}
	Log(LogLevel::Info) << "\tvvv ... revolution failed. :/";
}

void EU4::CountryManager::buildManufactoryCounts() const
{
	for (const auto& country: countries | std::views::values)
		country->buildManufactoryCount(countries);
}

void EU4::CountryManager::injectColorsIntoCountries()
{
	auto counter = 0;
	for (const auto& [tag, country]: countries)
	{
		const auto& color = commonCountryLoader.getCommonColor(tag);
		if (color && !country->getNationalColors().getMapColor())
		{
			country->setMapColor(*color);
			++counter;
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " countries needed updating.";
}

void EU4::CountryManager::mergeNations()
{
	if (nationMergeMapper.getMergeDaimyos())
		uniteJapan();

	for (const auto& mergeBlock: nationMergeMapper.getMergeBlocks())
		if (mergeBlock.shouldMerge() && !mergeBlock.getMaster().empty())
		{
			Log(LogLevel::Info) << "- Merging nations for: " << mergeBlock.getMaster();
			auto master = getCountry(mergeBlock.getMaster());
			if (!master)
			{
				Log(LogLevel::Warning) << "Country " << mergeBlock.getMaster() << " in configurables/merge_nations.txt does not exist in the save! Skipping.";
				continue;
			}
			for (const auto& slaveTag: mergeBlock.getSlaves())
			{
				const auto& slave = getCountry(slaveTag);
				if (!slave)
				{
					Log(LogLevel::Warning) << "Country " << slaveTag << " in configurables/merge_nations.txt does not exist in the save! Skipping.";
					continue;
				}
				master->eatCountry(slave);
			}
		}
}

void EU4::CountryManager::uniteJapan()
{
	Log(LogLevel::Info) << "-> Uniting Japan";

	// japan, right?
	auto japan = getCountry("JAP");
	if (!japan)
	{
		// no japan. look for japan.
		for (const auto& country: countries | std::views::values)
		{
			if (country->isPossibleShogun())
			{
				const auto& tag = country->getTag();
				Log(LogLevel::Info) << "- " << tag << " is the shogun.";
				japan = getCountry(tag);
				break;
			}
		}
	}

	for (const auto& country: countries | std::views::values)
		if (country->isPossibleDaimyo())
			japan->eatCountry(country);
}

void EU4::CountryManager::injectLocalizations()
{
	auto counter = 0;
	auto counterAdj = 0;
	for (const auto& theCountry: countries | std::views::values)
	{
		const auto& nameLocalizations = localizationLoader.getTextInEachLanguage(theCountry->getTag());
		if (nameLocalizations)
			for (const auto& [language, name]: *nameLocalizations)
			{
				theCountry->setLocalizationName(language, name);
				++counter;
			}

		const auto& adjectiveLocalizations = localizationLoader.getTextInEachLanguage(theCountry->getTag() + "_ADJ");
		if (adjectiveLocalizations)
			for (const auto& [language, adjective]: *adjectiveLocalizations)
			{
				theCountry->setLocalizationAdjective(language, adjective);
				++counterAdj;
			}
	}
	Log(LogLevel::Info) << "<> " << counter << " country names and " << counterAdj << " country adjectives loaded.";
}

void EU4::CountryManager::filterDeadNations(Configuration::DEADCORES toggle)
{
	removeEmptyNations();

	if (toggle == Configuration::DEADCORES::DeadCores)
	{
		removeDeadLandlessNations();
	}
	else if (toggle == Configuration::DEADCORES::AllCores)
	{
		removeLandlessNations();
	}
}

void EU4::CountryManager::removeEmptyNations()
{
	// Remove nations without provinces or cores. These are dead by definition as they can't be resurrected.

	auto counter = countries.size();
	std::map<std::string, std::shared_ptr<Country>> survivingCountries;

	for (const auto& country: countries)
	{
		const auto& countryProvinces = country.second->getProvinces();
		const auto& countryCores = country.second->getCores();
		if (!countryProvinces.empty() || !countryCores.empty())
			survivingCountries.insert(country);
	}

	countries.swap(survivingCountries);
	counter -= countries.size();

	Log(LogLevel::Info) << "<> " << counter << " empty nations disposed, " << countries.size() << " remain.";
}

void EU4::CountryManager::removeDeadLandlessNations()
{
	// This one removes nations that can't be resurrected from their cores because those provinces don't contain a majority of their
	// primary culture.
	auto counter = countries.size();

	std::map<std::string, std::shared_ptr<Country>> landlessCountries;
	for (const auto& country: countries)
		if (country.second->getProvinces().empty())
			landlessCountries.insert(country);

	for (const auto& country: landlessCountries)
		if (!country.second->cultureSurvivesInCores(countries))
			countries.erase(country.first);

	counter -= countries.size();
	Log(LogLevel::Info) << "<> " << counter << " dead landless nations disposed, " << countries.size() << " remain.";
}

void EU4::CountryManager::removeLandlessNations()
{
	// The most direct one - no land, no survival.

	auto counter = countries.size();
	std::map<std::string, std::shared_ptr<Country>> survivingCountries;

	for (const auto& country: countries)
		if (const auto& theProvinces = country.second->getProvinces(); !theProvinces.empty())
			survivingCountries.insert(country);

	countries.swap(survivingCountries);
	counter -= countries.size();
	Log(LogLevel::Info) << "<> " << counter << " landless nations disposed, " << countries.size() << " remain.";
}
