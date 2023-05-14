#include "EU4CountryManager.h"
#include "CommonRegexes.h"
#include "DiplomacyParser/DiplomacyParser.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "RegionManager/RegionManager.h"
#include <iomanip>
#include <ranges>

namespace
{
std::vector<std::string> sortMap(const std::map<std::string, double>& theMap)
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
} // namespace

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

void EU4::CountryManager::loadUnitTypes(const commonItems::ModFilesystem& modFS)
{
	unitTypeLoader.loadUnitTypes(modFS);
}

void EU4::CountryManager::loadCommonCountries(const commonItems::ModFilesystem& modFS)
{
	commonCountryLoader.loadCommonCountries(modFS);
}

void EU4::CountryManager::loadLocalizations(const commonItems::ModFilesystem& modFS)
{
	localizationLoader.loadLocalizations(modFS);
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

void EU4::CountryManager::assignGPStatuses()
{
	Log(LogLevel::Info) << "-> Assigning GP statuses.";
	std::map<std::string, double> prestigeScores;
	for (const auto& [tag, country]: countries)
	{
		if (country->getProvinces().empty())
			continue;
		prestigeScores.emplace(tag, country->getScore());
	}
	auto prestigeOrder = sortMap(prestigeScores);
	if (prestigeOrder.size() >= 8) // take the top 8 EU4 GPs.
		prestigeOrder = {prestigeOrder.begin() + prestigeOrder.size() - 8, prestigeOrder.end()};
	const std::set<std::string> prestigeTags = {prestigeOrder.begin(), prestigeOrder.end()};
	for (const auto& tag: prestigeTags)
		countries.at(tag)->setGP();

	Log(LogLevel::Info) << "<> GPs assigned to " << prestigeTags.size() << " countries.";
}

void EU4::CountryManager::splitTradeCompanies(const ProvinceManager& provinceManager,
	 const RegionManager& regionManager,
	 DiplomacyParser& diplomacyParser,
	 const Configuration::SPLITTCS splitTCs)
{
	if (splitTCs == Configuration::SPLITTCS::Disabled)
	{
		Log(LogLevel::Info) << "<> Trade Company splitoff disabled by configuration.";
	}
	else if (splitTCs == Configuration::SPLITTCS::TradeRegions)
	{
		splitTradeCompaniesByTradeRegions(provinceManager, regionManager, diplomacyParser);
	}
	else
	{
		splitTradeCompaniesBySuperRegions(provinceManager, regionManager, diplomacyParser);
	}
}

void EU4::CountryManager::splitTradeCompaniesBySuperRegions(const ProvinceManager& provinceManager,
	 const RegionManager& regionManager,
	 DiplomacyParser& diplomacyParser)
{
	auto counter = 0;

	// filter by regions and tags
	const auto& tcLoader = regionManager.getTradeCompanyLoader();
	std::map<std::string, std::map<std::string, std::set<int>>> tagRegionProvinces; // tag-> region->provinces

	for (const auto& [provinceID, province]: provinceManager.getAllProvinces())
	{
		if (!province->isTradeCompany())
			continue;
		// not yet. If this is one of blocked trade regions, then bail.
		const auto& tc = tcLoader.getTCForProvince(provinceID);
		if (!tc)
			continue;
		// ok, where are we?
		const auto& superRegionName = regionManager.getParentSuperRegionName(provinceID);
		if (!superRegionName)
			continue;

		if (!tagRegionProvinces.contains(province->getOwnerTag()))
			tagRegionProvinces.emplace(province->getOwnerTag(), std::map<std::string, std::set<int>>{});
		if (!tagRegionProvinces.at(province->getOwnerTag()).contains(*superRegionName))
			tagRegionProvinces.at(province->getOwnerTag()).emplace(*superRegionName, std::set<int>{});
		tagRegionProvinces.at(province->getOwnerTag()).at(*superRegionName).emplace(provinceID);
	}

	// Now.. Now now now. Single province, or 3, in a region should remain as is - as a trading port.
	// More than one should get cut. So let's cut.
	for (const auto& [tag, regionProvinces]: tagRegionProvinces)
	{
		for (const auto& [region, provinceIDs]: regionProvinces)
		{
			if (provinceIDs.size() <= 3)
				continue;

			std::map<int, std::shared_ptr<Province>> provinces;
			for (const auto& provinceID: provinceIDs)
				provinces.emplace(provinceID, provinceManager.getProvince(provinceID));
			generateSuperRegionTradeCompany(provinces, tag, region, diplomacyParser);
			++counter;
		}
	}
	Log(LogLevel::Info) << "<> Split " << counter << " trade companies from parents.";
}

void EU4::CountryManager::splitTradeCompaniesByTradeRegions(const ProvinceManager& provinceManager,
	 const RegionManager& regionManager,
	 DiplomacyParser& diplomacyParser)
{
	auto counter = 0;

	// filter by regions and tags
	const auto& tcLoader = regionManager.getTradeCompanyLoader();
	std::map<std::string, std::map<std::string, std::set<int>>> tagRegionProvinces; // tag-> region->provinces

	for (const auto& [provinceID, province]: provinceManager.getAllProvinces())
	{
		if (!province->isTradeCompany())
			continue;
		// not yet.
		const auto& tc = tcLoader.getTCForProvince(provinceID);
		if (!tc)
			continue;
		// ok, where are we?
		if (!tagRegionProvinces.contains(province->getOwnerTag()))
			tagRegionProvinces.emplace(province->getOwnerTag(), std::map<std::string, std::set<int>>{});
		if (!tagRegionProvinces.at(province->getOwnerTag()).contains(tc->name))
			tagRegionProvinces.at(province->getOwnerTag()).emplace(tc->name, std::set<int>{});
		tagRegionProvinces.at(province->getOwnerTag()).at(tc->name).emplace(provinceID);
	}

	// Now.. Now now now. Single province, or 4, in a region should remain as is - as a trading port.
	// More than one should get cut. So let's cut.
	for (const auto& [tag, regionProvinces]: tagRegionProvinces)
	{
		for (const auto& [region, provinceIDs]: regionProvinces)
		{
			if (provinceIDs.size() <= 4)
				continue;

			const auto& tc = tcLoader.getTradeCompany(region);
			std::map<int, std::shared_ptr<Province>> provinces;
			for (const auto& provinceID: provinceIDs)
				provinces.emplace(provinceID, provinceManager.getProvince(provinceID));
			generateTradeCompanyTradeCompany(provinces, tag, tc, diplomacyParser);
			++counter;
		}
	}
	Log(LogLevel::Info) << "<> Split " << counter << " trade companies from parents.";
}

void EU4::CountryManager::transferOwnerDataToTC(const std::shared_ptr<Country>& tradingCountry,
	 const std::string& ownerTag,
	 const std::map<int, std::shared_ptr<Province>>& provinces,
	 DiplomacyParser& diplomacyParser)
{
	const auto& tag = tradingCountry->getTag();
	const auto& owner = countries.at(ownerTag);
	for (const auto& [provinceID, province]: provinces)
	{
		// clear provinces
		owner->removeProvince(provinceID);
		// reassign provinces
		province->setOwnerTag(tag);
		province->setControllerTag(tag);
		province->addCore(tag);
		tradingCountry->addProvince(province);
		tradingCountry->addCore(province);
	}

	// lets transfer only what we actually need.
	tradingCountry->setAdmTech(owner->getAdmTech());
	tradingCountry->setDipTech(owner->getDipTech());
	tradingCountry->setMilTech(owner->getMilTech());
	tradingCountry->setTechGroup(owner->getTechGroup());
	tradingCountry->setPrimaryCulture(owner->getPrimaryCulture());
	tradingCountry->setReligion(owner->getReligion());
	tradingCountry->setFlags(owner->getFlags());
	tradingCountry->setModifiers(owner->getModifiers());
	tradingCountry->setGovernmentRank(1); // force to 1.
	tradingCountry->setReforms(owner->getReforms());
	tradingCountry->setNationalIdeas(owner->getNationalIdeas());
	tradingCountry->setEmbracedInstitutions(owner->getEmbracedInstitutions());
	tradingCountry->setNationalColors(owner->getNationalColors()); // same color. yah.

	// relations
	tradingCountry->setOverLord(ownerTag);
	auto relation = EU4RelationDetails();
	relation.setValue(75);
	tradingCountry->addRelation(ownerTag, relation);
	owner->addRelation(tag, relation);

	// subject status
	EU4Agreement agreement;
	agreement.setOriginTag(ownerTag);
	agreement.setTargetTag(tag);
	agreement.setAgreementType("private_enterprise");
	diplomacyParser.addAgreement(agreement);
}

void EU4::CountryManager::generateSuperRegionTradeCompany(const std::map<int, std::shared_ptr<Province>>& provinces,
	 const std::string& ownerTag,
	 const std::string& superRegionName,
	 DiplomacyParser& diplomacyParser)
{
	// Did we "accidentally" delete the owner?
	if (!countries.contains(ownerTag))
		return; // best not to dwell on this.

	const auto tag = generateNewTag();
	const auto tradingCountry = std::make_shared<Country>();
	tradingCountry->setTag(tag);

	transferOwnerDataToTC(tradingCountry, ownerTag, provinces, diplomacyParser);

	// For superregions, names are a simple affair.
	std::map<std::string, std::string> regionalNames;
	if (const auto& regionalLocBlock = localizationLoader.getTextInEachLanguage(superRegionName); regionalLocBlock)
		regionalNames = *regionalLocBlock;

	if (regionalNames.empty()) // should never happen!
		Log(LogLevel::Error) << "We cannot find localization for " << superRegionName << "! What happened to the locs?!";

	std::map<std::string, std::string> ownerNames;
	if (const auto& ownerLocBlock = localizationLoader.getTextInEachLanguage(ownerTag + "_ADJ"); ownerLocBlock)
		ownerNames = *ownerLocBlock;

	if (ownerNames.empty()) // should also never happen!
		Log(LogLevel::Error) << "We cannot find localization for " << ownerTag + "_ADJ! What happened to the locs?!";

	std::map<std::string, std::string> tradeCompanyNames;
	if (const auto& tcLocBlock = localizationLoader.getTextInEachLanguage("TRADE_COMPANY"); tcLocBlock)
		tradeCompanyNames = *tcLocBlock;

	if (tradeCompanyNames.empty()) // If this happened, someone overwrote entire game locs.
		Log(LogLevel::Error) << "We cannot find localization for " << TRADE_COMPANY + "! Your installation is broken.";

	for (const auto& [language, tradeName]: tradeCompanyNames)
	{
		if (ownerNames.contains(language) && regionalNames.contains(language))
		{
			std::string longName;	  // I am no linguist so I cannot do this properly.
			if (language == "french") // de, des, du.. I really can't.
			{
				longName = tradeName + " " + ownerNames.at(language) + " de " + regionalNames.at(language);
			}
			else if (language == "german") // not a clue if this is correct.
			{
				longName = ownerNames.at(language) + " " + regionalNames.at(language) + "-" + tradeName;
			}
			else if (language == "spanish") // ??? Maybe.
			{
				longName = tradeName + " " + ownerNames.at(language) + " de " + regionalNames.at(language);
			}
			else
			{
				longName = ownerNames.at(language) + " " + regionalNames.at(language) + " " + tradeName;
			}
			tradingCountry->setLocalizationName(language, longName);
			tradingCountry->setLocalizationAdjective(language, longName);
		}
	}
	countries.emplace(tag, tradingCountry);
}

void EU4::CountryManager::generateTradeCompanyTradeCompany(const std::map<int, std::shared_ptr<Province>>& provinces,
	 const std::string& ownerTag,
	 const TradeCompany& tc,
	 DiplomacyParser& diplomacyParser)
{
	// Did we "accidentally" delete the owner?
	if (!countries.contains(ownerTag))
		return; // best not to dwell on this.

	const auto tag = generateNewTag();
	const auto tradingCountry = std::make_shared<Country>();
	tradingCountry->setTag(tag);

	transferOwnerDataToTC(tradingCountry, ownerTag, provinces, diplomacyParser);
	if (tc.color)
		tradingCountry->setMapColor(*tc.color); // Let's try this for now.

	// Localizations are funny. First one is usually the one we want, the TRADE_COMPANY_SOUTH_AFRICA_Root_Culture_GetName
	// That expands in locmapper to: [Root.Culture.GetName] South Africa Company, which we have to manually insert.
	// Failing that, there's the second one: TRADE_COMPANY_SOUTH_AFRICA_Africa_Trade_Company
	// That's a straight: South Africa Trade Company, no brains required. First one is better, if present.
	// Failing everything, we generate.
	bool haveName = false;
	for (const auto& name: tc.locNameKeys)
	{
		if (const auto& locBlock = localizationLoader.getTextInEachLanguage(name); locBlock)
		{
			auto newBlock = *locBlock;
			if (locBlock->contains("english") && locBlock->at("english").find("[Root.") != std::string::npos)
			{
				// we need to alter.
				for (const auto& [language, longName]: *locBlock)
				{
					std::string insertText;
					if (const auto adj = localizationLoader.getTextForKey(ownerTag + "_ADJ", language); adj)
						insertText = *adj; // "DUTCH East India Company" because NET_ADJ="Dutch"
					else if (const auto& culName = localizationLoader.getTextForKey(countries.at(ownerTag)->getPrimaryCulture(), language); culName)
						insertText = *culName;

					if (!insertText.empty())
					{
						auto pos1 = longName.find("[");
						if (pos1 == std::string::npos)
						{
							newBlock.erase(language);
							continue;
						}
						auto pos2 = longName.find("]");
						if (pos2 == std::string::npos)
						{
							newBlock.erase(language);
							continue;
						}

						newBlock[language] = longName.substr(0, pos1) + insertText + longName.substr(pos2 + 1, longName.size());
					}
					else
					{
						newBlock.erase(language);
					}
				}
			}
			else
			{
				// we're in the second one. Can't do anything about that.
			}
			if (!newBlock.empty())
			{
				haveName = true;
				for (const auto& [language, longName]: newBlock)
				{
					tradingCountry->setLocalizationName(language, longName);
					tradingCountry->setLocalizationAdjective(language, longName);
				}
				break;
			}
		}
	}
	if (!haveName)
	{
		// Well shit. What are we even doing?
		if (const auto& countryAdj = localizationLoader.getTextForKey(ownerTag + "_ADJ", "english"); countryAdj)
		{
			tradingCountry->setLocalizationName("english", *countryAdj + " Trading Company");
			tradingCountry->setLocalizationAdjective("english", *countryAdj + " Trading Company");
		}
		else
		{
			// WTF time. We don't have any locs? NOT AN ISSUE! THIS WILL HAVE LOCS! THERE! THIS COUNTRY OF ALL COUNTRIES WILL HAVE LOCS!
			tradingCountry->setLocalizationName("english", "Trading Company");
			tradingCountry->setLocalizationAdjective("english", "Trading Company");
			// THERE! HAPPY?!
		}
	}

	// and FILE!
	countries.emplace(tag, tradingCountry);
}

std::string EU4::CountryManager::generateNewTag()
{
	std::string eu4Tag;
	do
	{
		std::ostringstream generatedV3TagStream;
		generatedV3TagStream << generatedEU4TagPrefix << std::setfill('0') << std::setw(2) << generatedEU4TagSuffix;
		eu4Tag = generatedV3TagStream.str();

		++generatedEU4TagSuffix;
		if (generatedEU4TagSuffix > 99)
		{
			generatedEU4TagSuffix = 0;
			--generatedEU4TagPrefix;
		}
	} while (countries.contains(eu4Tag));
	return eu4Tag;
}
