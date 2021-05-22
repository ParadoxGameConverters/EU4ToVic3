#include "Province.h"
#include "../../Configuration.h"
#include "../../EU4World/Regions/Regions.h"
#include "../../Mappers/CountryMappings/CountryMappings.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "../../Mappers/Geography/Continents.h"
#include "../../Mappers/ReligionMapper/ReligionMapper.h"
#include "../Army/Regiment.h"
#include "../Country/Country.h"
#include "CommonFunctions.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <algorithm>
#include <cmath>

V2::Province::Province(std::string _filename,
	 const mappers::ClimateMapper& climateMapper,
	 const mappers::TerrainDataMapper& terrainDataMapper,
	 const ProvinceNameParser& provinceNameParser,
	 const mappers::NavalBaseMapper& navalBaseMapper):
	 filename(std::move(_filename))
{
	const auto temp = trimPath(filename);
	try
	{
		provinceID = std::stoi(temp);
	}
	catch (std::exception& e)
	{
		Log(LogLevel::Error) << "Failed to extract province number from file name: " << filename << " " << e.what();
		throw std::runtime_error("Failed to extract province number from file name: " + filename + " " + e.what());
	}

	// In case we're overriding provinces (not true by default)
	if (commonItems::DoesFileExist("blankMod/output/history/provinces/" + filename))
	{
		details = mappers::ProvinceDetailsMapper("blankMod/output/history/provinces/" + filename).getProvinceDetails();
	}
	else
	{
		details = mappers::ProvinceDetailsMapper(theConfiguration.getVic2Path() + "/history/provinces/" + filename).getProvinceDetails();
	}

	for (const auto& climate: climateMapper.getClimateMap())
	{
		if (count(climate.second.begin(), climate.second.end(), provinceID))
		{
			details.climate = climate.first;
			break;
		}
	}

	if (details.terrain.empty() && !theConfiguration.isHpmEnabled())
	{
		auto terrain = terrainDataMapper.getTerrainForID(provinceID);
		if (terrain)
			details.terrain = *terrain;
	}

	auto potentialName = provinceNameParser.getProvinceName(provinceID);
	if (potentialName)
		name = *potentialName;

	if (navalBaseMapper.isProvinceCoastal(provinceID))
		coastal = true;
}

std::string V2::Province::getDominantCulture()
{
	std::map<std::string, long> census;
	for (const auto& pop: pops)
		census[pop->getCulture()] += pop->getSize();

	using pair_type = decltype(census)::value_type;
	const auto pr = std::max_element(std::begin(census), std::end(census), [](const pair_type& p1, const pair_type& p2) {
		return p1.second < p2.second;
	});
	return pr->first;
}

void V2::Province::addVanillaPop(std::shared_ptr<Pop> vanillaPop)
{
	vanillaPops.push_back(vanillaPop);
	vanillaPopulation += vanillaPop->getSize();
}

void V2::Province::addMinorityPop(std::shared_ptr<Pop> minorityPop)
{
	minorityPops.push_back(minorityPop);
}

void V2::Province::addCore(const std::string& newCore)
{
	// only add if not a territorial core/colony of the current owner
	if (!(newCore == details.owner && territorialCore))
	{
		details.cores.insert(newCore);
	}
}

void V2::Province::convertFromOldProvince(const std::vector<std::shared_ptr<EU4::Province>>& provinceSources,
	 const std::map<std::string, std::shared_ptr<EU4::Country>>& theEU4Countries,
	 const EU4::Regions& eu4Regions,
	 mappers::CultureMapper& cultureMapper,
	 const mappers::CultureMapper& slaveCultureMapper,
	 const mappers::Continents& continentsMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const mappers::CountryMappings& countryMapper,
	 const mappers::ProvinceMapper& provinceMapper,
	 bool hreDecentralized)
{
	// Drop vanilla cores
	details.cores.clear();

	if (provinceSources.empty())
		return; // Let's not do damage.

	// Store the numbers.
	for (const auto& oldProvince: provinceSources)
		eu4IDs.insert(oldProvince->getNum());

	// Single HRE province is enough
	for (const auto& oldProvince: provinceSources)
	{
		if (oldProvince->inHre())
		{
			inHRE = true;
			if (!hreDecentralized)
				addCore("HRE");
		}
	}
	
	territorialCore = false; // A single territorial core will be sufficient to trip this.
	for (const auto& oldProvince: provinceSources)
	{
		if (!oldProvince->isCity())
		{
			colonial = 1;
			territorialCore = true;
		}
		else if (oldProvince->isTerritorialCore())
		{
			colonial = 2;
			territorialCore = true;
		}
		else
		{
			colonial = 0;
		}
	}
	// Single colonized province is enough
	for (const auto& oldProvince: provinceSources)
		if (oldProvince->wasColonized())
			wasColonised = true;

	// For buildings, we go with averages.
	for (const auto& oldProvince: provinceSources)
	{
		// TODO: Dump buildings and values into own parser - duplication at EU4::Country
		if (oldProvince->hasBuilding("weapons"))
			++mfgCount;
		if (oldProvince->hasBuilding("wharf"))
			++mfgCount;
		if (oldProvince->hasBuilding("textile"))
			++mfgCount;
		if (oldProvince->hasBuilding("plantations"))
			++mfgCount;
		if (oldProvince->hasBuilding("tradecompany"))
			++mfgCount;
		if (oldProvince->hasBuilding("farm_estate"))
			++mfgCount;
		if (oldProvince->hasBuilding("mills"))
			++mfgCount;
		if (oldProvince->hasBuilding("furnace"))
			mfgCount += 3;
		// Shipyard and dock are worthless in terms of 19th century. However, grand shipyard and dry-dock are cumulative.
		if (oldProvince->hasBuilding("grand_shipyard"))
			++navalBaseLevel;
		if (oldProvince->hasBuilding("naval_base") && !oldProvince->hasBuilding("grand_shipyard"))
			++navalBaseLevel; // obsolete, but not cumulative if it appears
		if (oldProvince->hasBuilding("drydock"))
			++navalBaseLevel;
		// castle, bastion and star fort are worthless against 19th century artillery.
		if (oldProvince->hasBuilding("fort_18th"))
			++fortLevel;
		if (oldProvince->hasBuilding("fort6"))
			++fortLevel; // obsolete, exclusive with fort_18th_century

		// Add up different building types.
		auto provinceBuildings = oldProvince->getBuildings();
		importedBuildings.insert(provinceBuildings.begin(), provinceBuildings.end());
	}
	mfgCount = lround(mfgCount / provinceSources.size()); // Since these are built everywhere, use average.
	navalBaseLevel = std::min(navalBaseLevel, 2);			// Don't go over 2 for naval bases.
	fortLevel = std::min(fortLevel, 1);							// Don't go over 1 for forts.

	const auto& countryItr = theEU4Countries.find(provinceSources[0]->getOwnerString());
	if (countryItr != theEU4Countries.end())
	{
		importedIdeas = countryItr->second->exportNationalIdeas();
	}

	for (const auto& oldProvince: provinceSources)
		for (const auto& core: oldProvince->getCores())
		{
			auto potentialCore = countryMapper.getV2Tag(core);
			if (potentialCore)
				addCore(*potentialCore);
		}

	determineColonial(); // Sanity check at most, we would probably be ok without it.

	// onto development and weight data

	// Extreme rebalancing note:
	// When mapping M-to-N, generally speaking, we're summing up all incoming development, and distributing
	// it across target provinces. This way we maintain total investment and aren't wasting dev.
	//
	// Devpush shaping note:
	// For devpush we're averaging our investment factor from the incoming provinces, without regard to other
	// provinces around us.
	std::set<int> coTargets; // These would be all the target provinces in the same mapping link.
	for (const auto& oldProvince: provinceSources)
	{
		const auto& targets = provinceMapper.getVic2ProvinceNumbers(oldProvince->getNum());
		coTargets.insert(targets.begin(), targets.end());
		investmentFactor += oldProvince->getInvestmentFactor() / 100.0;
		provinceWeight += oldProvince->getProvinceWeight();
	}
	investmentFactor /= static_cast<double>(provinceSources.size());
	const auto totalSourceDevelopmentWeight = provinceWeight;

	provinceWeight /= static_cast<double>(coTargets.size()); // dividing by target provinces.

	// And finally, demographics
	for (const auto& oldProvince: provinceSources)
	{
		const auto provincePopulationWeight = oldProvince->getProvinceWeight() / totalSourceDevelopmentWeight;
		auto popRatios = oldProvince->getPopRatios();
		determineDemographics(eu4Regions,
			 popRatios,
			 oldProvince->getNum(),
			 oldProvince->getOwnerString(),
			 provinceID,
			 provincePopulationWeight,
			 cultureMapper,
			 slaveCultureMapper,
			 continentsMapper,
			 religionMapper);
	}
}

void V2::Province::sterilizeProvince()
{
	details.owner = "";
	details.controller = "";
	details.cores.clear();
	details.colonial = 0;
	details.colonyLevel = 0;
	details.navalBaseLevel = 0;
	details.fortLevel = 0;
	details.railLevel = 0;
}

void V2::Province::determineDemographics(const EU4::Regions& eu4Regions,
	 std::vector<EU4::PopRatio>& popRatios,
	 int eu4ProvID,
	 const std::string& oldOwnerTag,
	 int destNum,
	 double provPopRatio,
	 mappers::CultureMapper& cultureMapper,
	 const mappers::CultureMapper& slaveCultureMapper,
	 const mappers::Continents& continentsMapper,
	 const mappers::ReligionMapper& religionMapper)
{
	for (const auto& popRatio: popRatios)
	{
		auto dstCulture = cultureMapper.cultureMatch(eu4Regions, popRatio.getCulture(), popRatio.getReligion(), eu4ProvID, oldOwnerTag);
		if (!dstCulture)
		{
			// No panic, yet. We may be dealing with a neoculture.
			if (!popRatio.getOriginalCulture().empty())
			{
				// This is a neoculture. Failure to map is not an option. Locate a mapping based on original culture if one exists, but ping for
				// area, region or superregion. We're not interested in general mappings.
				dstCulture = cultureMapper.cultureRegionalMatch(eu4Regions, popRatio.getOriginalCulture(), popRatio.getReligion(), eu4ProvID, oldOwnerTag);
				if (!dstCulture)
				{
					// There is no overriding rule. We're good to force neoculture.
					generatedNeoCultures.insert(std::make_pair(popRatio.getOriginalCulture(), popRatio.getCulture()));
					superRegion = popRatio.getSuperRegion();
					dstCulture.emplace(popRatio.getCulture());
				}
			}
		}

		if (!dstCulture)
		{
			LOG(LogLevel::Warning) << "Could not convert eu4 culture " << popRatio.getCulture() << " for pops in Vic2 province " << destNum
										  << "! Check mappings, substituting noculture.";
			dstCulture.emplace("noculture");
		}
		else if (*dstCulture == "noculture")
		{
			LOG(LogLevel::Warning) << "Incoming eu4 noculture pops for Vic2 province " << destNum
										  << "! Your EU4 save seems borked there, troubles with CK2 import?";
		}

		auto religion = religionMapper.getVic2Religion(popRatio.getReligion());
		if (!religion)
		{
			LOG(LogLevel::Warning) << "Could not convert eu4 religion " << popRatio.getReligion() << " for pops in Vic2 province " << destNum
										  << "! Check mappings, substituting no_religion.";
			religion.emplace("noreligion");
		}
		else if (*religion == "noreligion")
		{
			LOG(LogLevel::Warning) << "Incoming eu4 noreligion pops for Vic2 province " << destNum
										  << "! Your EU4 save seems borked there, troubles with CK2 import?";
		}

		auto slaveCulture = slaveCultureMapper.cultureMatch(eu4Regions, popRatio.getCulture(), popRatio.getReligion(), eu4ProvID, oldOwnerTag);
		auto thisContinent = continentsMapper.getEU4Continent(eu4ProvID);
		if (!slaveCulture && !popRatio.getOriginalCulture().empty())
			slaveCulture = slaveCultureMapper.cultureMatch(eu4Regions, popRatio.getOriginalCulture(), popRatio.getReligion(), eu4ProvID, oldOwnerTag);
		if (!slaveCulture)
		{
			if (thisContinent && (*thisContinent == "asia" || *thisContinent == "africa" || *thisContinent == "oceania"))
			{
				LOG(LogLevel::Warning) << "No mapping for slave culture " << popRatio.getCulture() << "(" << popRatio.getOriginalCulture() << ")/"
											  << popRatio.getReligion() << " in province " << destNum << "/" << *thisContinent << " - using native culture ("
											  << popRatio.getCulture() << ").";
				slaveCulture.emplace(popRatio.getCulture());
			}
			else
			{
				LOG(LogLevel::Warning) << "No mapping for slave culture " << popRatio.getCulture() << "(" << popRatio.getOriginalCulture() << ")/"
											  << popRatio.getReligion() << " for pops in Vic2 province " << destNum << "/" << *thisContinent << " - using african_minor.";
				slaveCulture.emplace("african_minor");
			}
		}

		Demographic demographic;
		demographic.culture = *dstCulture;
		demographic.slaveCulture = *slaveCulture;
		demographic.religion = *religion;
		demographic.upperRatio = popRatio.getUpperRatio() * provPopRatio;
		demographic.middleRatio = popRatio.getMiddleRatio() * provPopRatio;
		demographic.lowerRatio = popRatio.getLowerRatio() * provPopRatio;

		if (theConfiguration.getDebug())
		{
			LOG(LogLevel::Info) << "EU4 Province " << eu4ProvID << ", "
									  << "Vic2 Province " << provinceID << ", "
									  << "Culture: " << demographic.culture << ", "
									  << "Religion: " << demographic.religion << ", "
									  << "upperPopRatio: " << popRatio.getUpperRatio() << ", "
									  << "middlePopRatio: " << popRatio.getMiddleRatio() << ", "
									  << "lowerPopRatio: " << popRatio.getLowerRatio() << ", "
									  << "provPopRatio: " << provPopRatio << ", "
									  << "upperRatio: " << demographic.upperRatio << ", "
									  << "middleRatio: " << demographic.middleRatio << ", "
									  << "lowerRatio: " << demographic.lowerRatio;
		}
		demographics.push_back(demographic);
	}
}

std::optional<std::shared_ptr<V2::Factory>> V2::Province::addFactory(std::shared_ptr<Factory> factory)
{
	auto itr = factories.find(factory->getTypeName());
	if (itr == factories.end())
	{
		factories.insert(std::make_pair(factory->getTypeName(), factory));
		return std::move(factory);
	}
	itr->second->increaseLevel();
	return std::nullopt;
}

void V2::Province::addPopDemographic(const Demographic& d)
{
	auto combined = false;
	for (auto& demographic: demographics)
	{
		if (demographic.culture == d.culture && demographic.religion == d.religion)
		{
			combined = true;
			demographic.upperRatio += d.upperRatio;
			demographic.middleRatio += d.middleRatio;
			demographic.lowerRatio += d.lowerRatio;
		}
	}
	if (!combined)
	{
		demographics.push_back(d);
	}
}

void V2::Province::determineColonial()
{
	if (territorialCore && colonial == 0)
		colonial = 2;
}

int V2::Province::getTotalPopulation() const
{
	auto total = 0;
	for (const auto& pop: pops)
		total += pop->getSize();
	return total;
}

std::vector<std::string> V2::Province::getCulturesOverThreshold(double percentOfPopulation) const
{
	const auto totalPopulation = getTotalPopulation();
	if (!totalPopulation)
		return std::vector<std::string>();

	std::map<std::string, int> cultureTotals;
	for (const auto& pop: pops)
		cultureTotals[pop->getCulture()] += pop->getSize();

	std::vector<std::string> culturesOverThreshold;
	for (const auto& cultureAmount: cultureTotals)
	{
		if (static_cast<double>(cultureAmount.second) / totalPopulation >= percentOfPopulation)
		{
			culturesOverThreshold.push_back(cultureAmount.first);
		}
	}

	return culturesOverThreshold;
}

std::optional<std::pair<int, std::vector<std::shared_ptr<V2::Pop>>>> V2::Province::getPopsForOutput() const
{
	// TODO: This functionality is wrong. We don't need vanilla pops but customized pops from surrounding areas.
	if (resettable && theConfiguration.getResetProvinces() == "yes" && !vanillaPops.empty())
	{
		return std::pair(provinceID, vanillaPops);
	}
	if (!pops.empty())
		return std::pair(provinceID, pops);
	if (!vanillaPops.empty())
		return std::pair(provinceID, vanillaPops);
	return std::nullopt;
}

void V2::Province::doCreatePops(const double popWeightRatio,
	 Country* _owner,
	 const CIV_ALGORITHM popConversionAlgorithm,
	 const mappers::ProvinceMapper& provinceMapper)
{
	// convert pops
	for (const auto& demographic: demographics)
	{
		createPops(demographic, popWeightRatio, _owner, popConversionAlgorithm, provinceMapper);
	}
	combinePops();
	// organize pops for adding minorities
	std::map<std::string, int> totals;
	std::map<std::string, std::vector<std::shared_ptr<Pop>>> thePops;
	for (const auto& pop: pops)
	{
		auto type = pop->getType();

		totals[type] += pop->getSize();
		thePops[type].push_back(pop);
	}

	// decrease non-minority pops and create the minorities
	std::vector<std::shared_ptr<Pop>> actualMinorities;
	for (const auto& minority: minorityPops)
	{
		const auto totalTypePopulation = totals[minority->getType()];

		auto thePopsItr = thePops.find(minority->getType());
		if (thePopsItr != thePops.end())
		{
			for (const auto& popsItr: thePopsItr->second)
			{
				auto newCulture = minority->getCulture();
				auto newReligion = minority->getReligion();
				if (newCulture.empty())
					newCulture = popsItr->getCulture();
				if (newReligion.empty())
					newReligion = popsItr->getReligion();

				auto newMinority =
					 std::make_shared<Pop>(minority->getType(), lround(popsItr->getSize() / totalTypePopulation * minority->getSize()), newCulture, newReligion);
				actualMinorities.push_back(newMinority);

				popsItr->changeSize(static_cast<int>(-1.0 * popsItr->getSize() / totalTypePopulation * minority->getSize()));
			}
		}
	}

	// add minority pops to the main pops
	for (const auto& minority: actualMinorities)
	{
		pops.push_back(minority);
	}
	combinePops();
}

V2::Province::pop_points V2::Province::getPopPoints_1(const Demographic& demographic, const double newPopulation, const Country* _owner) const
{
	pop_points pts;

	auto govBuilding = 0;
	if (importedBuildings.count("temple"))
	{
		govBuilding = 1;
	}
	else if (importedBuildings.count("courthouse"))
	{
		govBuilding = 2;
	}
	else if (importedBuildings.count("spy_agency"))
	{
		govBuilding = 3;
	}
	else if (importedBuildings.count("town_hall"))
	{
		govBuilding = 4;
	}
	else if (importedBuildings.count("college"))
	{
		govBuilding = 6;
	}
	else if (importedBuildings.count("cathedral"))
	{
		govBuilding = 8;
	}

	auto armyBuilding = 0;
	if (importedBuildings.count("armory"))
	{
		armyBuilding = 1;
	}
	else if (importedBuildings.count("training_fields"))
	{
		armyBuilding = 2;
	}
	else if (importedBuildings.count("barracks"))
	{
		armyBuilding = 3;
	}
	else if (importedBuildings.count("regimental_camp"))
	{
		armyBuilding = 4;
	}
	else if (importedBuildings.count("arsenal"))
	{
		armyBuilding = 6;
	}
	else if (importedBuildings.count("conscription_center"))
	{
		armyBuilding = 8;
	}
	if (importedBuildings.count("ramparts"))
		armyBuilding += 4;
	if (importedBuildings.count("soldier_households"))
		armyBuilding += 8;
	if (importedBuildings.count("impressment_offices"))
		armyBuilding += 4;
	if (importedBuildings.count("coastal_defence"))
		armyBuilding += 4;

	auto productionBuilding = 0;
	if (importedBuildings.count("constable"))
	{
		productionBuilding = 1;
	}
	else if (importedBuildings.count("workshop"))
	{
		productionBuilding = 2;
	}
	else if (importedBuildings.count("counting_house"))
	{
		productionBuilding = 3;
	}
	else if (importedBuildings.count("treasury_office"))
	{
		productionBuilding = 4;
	}
	else if (importedBuildings.count("mint"))
	{
		productionBuilding = 6;
	}
	else if (importedBuildings.count("stock_exchange"))
	{
		productionBuilding = 8;
	}

	auto tradeBuilding = 0;
	if (importedBuildings.count("marketplace"))
	{
		tradeBuilding = 1;
	}
	else if (importedBuildings.count("trade_depot"))
	{
		tradeBuilding = 2;
	}
	else if (importedBuildings.count("canal"))
	{
		tradeBuilding = 3;
	}
	else if (importedBuildings.count("road_network"))
	{
		tradeBuilding = 4;
	}
	else if (importedBuildings.count("post_office"))
	{
		tradeBuilding = 6;
	}
	else if (importedBuildings.count("customs_house"))
	{
		tradeBuilding = 8;
	}

	pts.artisans += 400;
	pts.artisans += static_cast<double>(productionBuilding) * 125;

	pts.soldiers += 100;
	pts.soldiers += static_cast<double>(armyBuilding) * 45;
	if (importedIdeas.count("quantity_ideas"))
		pts.soldiers *= 2;

	pts.officers += 2 * (static_cast<double>(armyBuilding) + 2);
	if (importedIdeas.count("quality_ideas"))
		pts.officers += 5;

	pts.clergymen += 95;
	if (importedIdeas.count("religious_ideas"))
		pts.clergymen += 10;
	if (importedIdeas.count("innovative_ideas"))
		pts.clergymen += 10;

	pts.bureaucrats += 10;
	pts.bureaucrats += static_cast<double>(govBuilding) * 2;
	if (importedIdeas.count("administrative_ideas"))
		pts.bureaucrats += 10;
	if (importedIdeas.count("expansion_ideas") && wasColonised)
		pts.bureaucrats += 10;
	if (importedBuildings.count("state_house"))
		pts.bureaucrats *= 2;

	pts.aristocrats += 7 * (static_cast<double>(tradeBuilding) + 11);
	if (importedBuildings.count("farm_estate") || importedBuildings.count("plantations"))
		pts.aristocrats *= 2;
	if (importedIdeas.count("aristocracy_ideas"))
		pts.aristocrats *= 2;

	if (!factories.empty())
	{
		const auto capsPerFactory = 40 + static_cast<double>(_owner->getNumFactories()) * 2;
		const auto actualCapitalists =
			 static_cast<double>(factories.size()) * static_cast<double>(_owner->getNumFactories()) * capsPerFactory * demographic.upperRatio;
		pts.capitalists += 10000 * actualCapitalists / (demographic.upperRatio * newPopulation);

		const auto actualClerks = 181 * static_cast<double>(factories.size()) * demographic.middleRatio;
		pts.clerks += 10000 * actualClerks / (demographic.middleRatio * newPopulation);

		const auto actualCraftsmen = 2639 * static_cast<double>(factories.size()) * demographic.lowerRatio;
		pts.craftsmen += 10000 * actualCraftsmen / (demographic.lowerRatio * newPopulation);
	}
	return pts;
}

V2::Province::pop_points V2::Province::getPopPoints_2(const Demographic& demographic, const double newPopulation, const Country* _owner) const
{
	pop_points pts;

	auto adminBuilding = 0;
	if (importedBuildings.count("courthouse"))
	{
		adminBuilding = 1;
	}
	else if (importedBuildings.count("town_hall"))
	{
		adminBuilding = 2;
	}

	auto taxBuilding = 0;
	if (importedBuildings.count("temple"))
	{
		taxBuilding = 1;
	}
	else if (importedBuildings.count("cathedral"))
	{
		taxBuilding = 2;
	}

	auto manpowerBuilding = 0;
	if (importedBuildings.count("barracks"))
	{
		manpowerBuilding = 1;
	}
	else if (importedBuildings.count("training_fields"))
	{
		manpowerBuilding = 2;
	}

	auto armyBuilding = 0;
	if (importedBuildings.count("regimental_camp"))
	{
		armyBuilding = 1;
	}
	else if (importedBuildings.count("conscription_center"))
	{
		armyBuilding = 2;
	}

	auto productionBuilding = 0;
	if (importedBuildings.count("workshop"))
	{
		productionBuilding = 1;
	}
	else if (importedBuildings.count("counting_house"))
	{
		productionBuilding = 2;
	}

	auto tradeBuilding = 0;
	if (importedBuildings.count("marketplace"))
	{
		tradeBuilding = 1;
	}
	else if (importedBuildings.count("trade_depot"))
	{
		tradeBuilding = 2;
	}
	else if (importedBuildings.count("stock_exchange"))
	{
		tradeBuilding = 3;
	}

	pts.artisans += 400;
	pts.artisans += static_cast<double>(productionBuilding) * 500;

	pts.soldiers += 100;
	pts.soldiers += (static_cast<double>(manpowerBuilding) + static_cast<double>(armyBuilding)) * 90;
	if (importedIdeas.count("quantity_ideas"))
		pts.soldiers *= 2;

	pts.officers += 4 * (static_cast<double>(manpowerBuilding) + static_cast<double>(armyBuilding) + 2.0);
	if (importedIdeas.count("quality_ideas"))
		pts.officers += 5;

	pts.clergymen += 65;
	if (importedIdeas.count("religious_ideas"))
		pts.clergymen += 10;
	if (importedIdeas.count("innovative_ideas"))
		pts.clergymen += 10;
	if (importedBuildings.count("university"))
		pts.clergymen *= 2;

	pts.bureaucrats += 10;
	pts.bureaucrats += (static_cast<double>(adminBuilding) + static_cast<double>(taxBuilding)) * 4;
	if (importedIdeas.count("administrative_ideas"))
		pts.bureaucrats += 10;
	if (importedIdeas.count("expansion_ideas") && wasColonised)
		pts.bureaucrats += 10;

	pts.aristocrats += 14 * (static_cast<double>(tradeBuilding) + 6.0);
	if (importedBuildings.count("farm_estate") || importedBuildings.count("plantations"))
		pts.aristocrats *= 2;
	if (importedIdeas.count("aristocracy_ideas"))
		pts.aristocrats *= 2;

	if (!factories.empty())
	{
		auto const capsPerFactory = 40 + static_cast<double>(_owner->getNumFactories()) * 2;
		auto const actualCapitalists =
			 static_cast<double>(factories.size()) * static_cast<double>(_owner->getNumFactories()) * capsPerFactory * demographic.upperRatio;
		pts.capitalists += 10000 * actualCapitalists / (demographic.upperRatio * newPopulation);

		auto const actualClerks = 493 * static_cast<double>(factories.size()) * demographic.middleRatio;
		pts.clerks += 10000 * actualClerks / (demographic.middleRatio * newPopulation);

		auto const actualCraftsmen = 8170 * static_cast<double>(factories.size()) * demographic.lowerRatio;
		pts.craftsmen += 10000 * actualCraftsmen / (demographic.lowerRatio * newPopulation);
	}
	return pts;
}

void V2::Province::createPops(const Demographic& demographic,
	 double popWeightRatio,
	 const Country* _owner,
	 CIV_ALGORITHM popConversionAlgorithm,
	 const mappers::ProvinceMapper& provinceMapper)
{
	long newPopulation = 0;
	const auto shapeFactor = theConfiguration.getPopShapingFactor() / 100.0;
	const auto lifeRatingMod = (static_cast<double>(details.lifeRating) - 35.0) / 200.0;
	const auto provinceDevModifier = 1 + (lifeRatingMod + investmentFactor * shapeFactor);

	switch (theConfiguration.getPopShaping())
	{
		case Configuration::POPSHAPES::Vanilla:
			newPopulation = vanillaPopulation;
			break;

		case Configuration::POPSHAPES::PopShaping:
			newPopulation = static_cast<long>(vanillaPopulation * provinceDevModifier);
			break;

		case Configuration::POPSHAPES::Extreme:
			newPopulation = static_cast<long>(popWeightRatio * provinceWeight);
			newPopulation = vanillaPopulation + static_cast<long>((newPopulation - vanillaPopulation) * shapeFactor);
			break;
	}

	pop_points pts;
	switch (popConversionAlgorithm)
	{
		case CIV_ALGORITHM::older:
			pts = getPopPoints_1(demographic, newPopulation, _owner);
			break;
		case CIV_ALGORITHM::newer:
			pts = getPopPoints_2(demographic, newPopulation, _owner);
			break;
	}

	// Uncivilized cannot have capitalists, clerks, or craftsmen, and get fewer bureaucrats
	if (!_owner->isCivilized())
	{
		pts.capitalists = 0;
		pts.clerks = 0;
		pts.craftsmen = 0;
		pts.bureaucrats -= 5;
	}

	int farmers = lround(demographic.lowerRatio * newPopulation);
	if (slaveProportion > 0.0)
	{
		int size = lround(demographic.lowerRatio * newPopulation * slaveProportion);
		farmers -= size;
		auto slavesPop = std::make_shared<Pop>("slaves", size, demographic.slaveCulture, demographic.religion);
		pops.push_back(slavesPop);
	}
	if (pts.soldiers > 0)
	{
		int size = lround(demographic.lowerRatio * newPopulation * (pts.soldiers / 10000));
		farmers -= size;
		auto soldiersPop = std::make_shared<Pop>("soldiers", size, demographic.culture, demographic.religion);
		pops.push_back(soldiersPop);
	}
	if (pts.craftsmen > 0)
	{
		int size = lround(demographic.lowerRatio * newPopulation * (pts.craftsmen / 10000));
		farmers -= size;
		auto craftsmenPop = std::make_shared<Pop>("craftsmen", size, demographic.culture, demographic.religion);
		pops.push_back(craftsmenPop);
	}
	if (pts.artisans > 0)
	{
		int size = lround(demographic.middleRatio * newPopulation * (pts.artisans / 10000));
		farmers -= size;
		auto artisansPop = std::make_shared<Pop>("artisans", size, demographic.culture, demographic.religion);
		pops.push_back(artisansPop);
	}
	if (pts.clergymen > 0)
	{
		int size = lround(demographic.middleRatio * newPopulation * (pts.clergymen / 10000));
		farmers -= size;
		auto clergymenPop = std::make_shared<Pop>("clergymen", size, demographic.culture, demographic.religion);
		pops.push_back(clergymenPop);
	}
	if (pts.clerks > 0)
	{
		int size = lround(demographic.middleRatio * newPopulation * (pts.clerks / 10000));
		farmers -= size;
		auto clerksPop = std::make_shared<Pop>("clerks", size, demographic.culture, demographic.religion);
		pops.push_back(clerksPop);
	}
	if (pts.bureaucrats > 0)
	{
		int size = lround(demographic.middleRatio * newPopulation * (pts.bureaucrats / 10000));
		farmers -= size;
		auto bureaucratsPop = std::make_shared<Pop>("bureaucrats", size, demographic.culture, demographic.religion);
		pops.push_back(bureaucratsPop);
	}
	if (pts.officers > 0)
	{
		int size = lround(demographic.middleRatio * newPopulation * (pts.officers / 10000));
		farmers -= size;
		auto officersPop = std::make_shared<Pop>("officers", size, demographic.culture, demographic.religion);
		pops.push_back(officersPop);
	}
	if (pts.capitalists > 0)
	{
		int size = lround(demographic.upperRatio * newPopulation * (pts.capitalists / 10000));
		farmers -= size;
		auto capitalistsPop = std::make_shared<Pop>("capitalists", size, demographic.culture, demographic.religion);
		pops.push_back(capitalistsPop);
	}
	if (pts.aristocrats > 0)
	{
		int size = lround(demographic.upperRatio * newPopulation * (pts.aristocrats / 10000));
		farmers -= size;
		auto aristocratsPop = std::make_shared<Pop>("aristocrats", size, demographic.culture, demographic.religion);
		pops.push_back(aristocratsPop);
	}

	auto farmersPop = std::make_shared<Pop>("farmers", farmers, demographic.culture, demographic.religion);
	pops.push_back(farmersPop);
}

void V2::Province::combinePops()
{
	for (auto lhs = pops.begin(); lhs != pops.end(); ++lhs)
	{
		if ((*lhs)->isTrashed())
			continue;
		auto rhs = lhs;
		for (++rhs; rhs != pops.end(); ++rhs)
		{
			if ((*rhs)->isTrashed())
				continue;
			if ((*lhs)->combine(**rhs))
			{
				(*rhs)->setTrashed();
				continue;
			}
			if ((*rhs)->getSize() < 1)
			{
				(*rhs)->setTrashed();
			}
		}
	}

	std::vector<std::shared_ptr<Pop>> consolidatedPops;
	for (const auto& pop: pops)
	{
		if (pop->isTrashed())
			continue;
		consolidatedPops.emplace_back(pop);
	}
	pops.swap(consolidatedPops);
}

// pick a soldier pop to use for an army.  prefer larger pops to smaller ones, and grow only if necessary.
std::shared_ptr<V2::Pop> V2::Province::getSoldierPopForArmy(const bool force)
{
	auto soldierPops = getPops("soldiers");
	if (soldierPops.empty())
		return nullptr; // no soldier pops

	std::sort(soldierPops.begin(), soldierPops.end(), popSortBySizePredicate);
	// try largest to smallest, without growing
	for (auto soldier: soldierPops)
	{
		const auto growBy = getRequiredPopForRegimentCount(soldier->getSupportedRegimentCount() + 1) - soldier->getSize();
		if (growBy <= 0)
		{
			if (growSoldierPop(*soldier)) // won't actually grow, but will increment supported regiment count
			{
				return soldier;
			}
		}
	}
	// try largest to smallest, trying to grow
	for (const auto& soldier: soldierPops)
	{
		if (growSoldierPop(*soldier)) // Will actually grow and increment supported regiment count
		{
			return soldier;
		}
	}

	// no suitable pops
	if (force)
		return soldierPops[0];
	return nullptr;
}

std::vector<std::shared_ptr<V2::Pop>> V2::Province::getPops(const std::string& type) const
{
	std::vector<std::shared_ptr<Pop>> retval;
	for (const auto& pop: pops)
	{
		if (type == "*" || pop->getType() == type)
			retval.push_back(pop);
	}
	return retval;
}

bool V2::Province::popSortBySizePredicate(std::shared_ptr<Pop> pop1, std::shared_ptr<Pop> pop2)
{
	return pop1->getSize() > pop2->getSize();
}

// V2 requires 1000 for the first regiment and 3000 thereafter
// we require an extra 1/30 to stabilize the start of the game
int V2::Province::getRequiredPopForRegimentCount(const int count)
{
	if (count == 0)
		return 0;
	return 1033 + (count - 1) * 3100;
}

bool V2::Province::growSoldierPop(Pop& pop)
{
	const auto growBy = getRequiredPopForRegimentCount(pop.getSupportedRegimentCount() + 1) - pop.getSize();
	if (growBy > 0)
	{
		// gotta grow; find a same-culture same-religion farmer/laborer to pull from
		const auto provincePop = getTotalPopulation();
		auto foundSourcePop = false;
		for (auto& popSource: pops)
		{
			if (popSource->getType() == "farmers" || popSource->getType() == "labourers")
			{
				if (popSource->getCulture() == pop.getCulture() && popSource->getReligion() == pop.getReligion())
				{
					// don't let the farmer/laborer shrink beneath 10% of the province population
					if (static_cast<double>(popSource->getSize()) - growBy > provincePop * 0.10)
					{
						popSource->changeSize(-growBy);
						pop.changeSize(growBy);
						foundSourcePop = true;
						break;
					}
				}
			}
		}
		if (!foundSourcePop)
			return false;
	}
	pop.incrementSupportedRegimentCount();
	return true;
}

std::string V2::Province::getRegimentName(const REGIMENTTYPE chosenType)
{
	std::stringstream str;
	str << ++unitNameCount[chosenType] << cardinalToOrdinal(unitNameCount[chosenType]); // 1st, 2nd, etc
	str << " " << name << " ";																				// Hamburg, Lyon, etc
	switch (chosenType)
	{
		case REGIMENTTYPE::irregular:
			str << "Irregulars";
			break;
		case REGIMENTTYPE::infantry:
			str << "Infantry";
			break;
		case REGIMENTTYPE::cavalry:
			str << "Cavalry";
			break;
		case REGIMENTTYPE::artillery:
			str << "Artillery";
			break;
		case REGIMENTTYPE::manowar:
			str << "Man'o'war";
			break;
		case REGIMENTTYPE::frigate:
			str << "Frigate";
			break;
		case REGIMENTTYPE::clipper_transport:
			str << "Clipper Transport";
			break;
	}
	return str.str();
}

std::pair<int, int> V2::Province::getAvailableSoldierCapacity() const
{
	auto soldierCap = 0;
	auto draftCap = 0;
	const auto provincePop = getTotalPopulation();
	for (const auto& soldier: pops)
	{
		if (soldier->getType() == "soldiers")
		{
			// unused capacity is the size of the pop minus the capacity already used, or 0, if it's already overdrawn
			soldierCap += std::max(soldier->getSize() - getRequiredPopForRegimentCount(soldier->getSupportedRegimentCount()), 0);
		}
		else if (soldier->getType() == "farmers" || soldier->getType() == "labourers")
		{
			// unused capacity is the size of the pop in excess of 10% of the province pop, or 0, if it's already too small
			draftCap += std::max(soldier->getSize() - int(0.10 * provincePop), 0);
		}
	}
	return std::pair<int, int>(soldierCap, draftCap);
}
