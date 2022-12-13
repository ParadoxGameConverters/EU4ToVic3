#include "ClayManager/ClayManager.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "CountryManager/EU4Country.h"
#include "CountryMapper/CountryMapper.h"
#include "CultureLoader/CultureLoader.h"
#include "CultureMapper/CultureMapper.h"
#include "PoliticalManager/PoliticalManager.h"
#include "PopManager/PopManager.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "ReligionLoader/ReligionLoader.h"
#include "ReligionMapper/ReligionMapper.h"
#include "gtest/gtest.h"
#include <Log.h>
#include <gmock/gmock-matchers.h>

namespace
{
std::tuple<V3::ClayManager, V3::PoliticalManager> prepManagers()
{
	auto eu4Path = "TestFiles/eu4installation/";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(eu4Path, {});
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(eu4Path, {});
	EU4::RegionManager regionMapper;
	regionMapper.loadRegions(eu4Path, {});

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";																																		// sea, no ownership
	provinceStream << "-2={ owner = TA2 base_tax=10 base_production=10 base_manpower=10 culture = culture1 religion = catholic }\n"; // substate TA-2&3
	provinceStream << "-3={ owner = TA3 base_tax=1 base_production=1 base_manpower=1 culture = culture2 religion = protestant }\n";	// substate TA-2&3
	provinceStream << "-4={}\n";																																		// irrelevant
	provinceStream << "-5={}\n";																																		// irrelevant
	provinceStream << "-6={}\n";																																		// irrelevant
	provinceStream << "-7={}\n";																																		// irrelevant
	provinceStream << "-8={}\n";																																		// irrelevant
	provinceStream << "-9={ owner = TA9 base_tax=1 base_production=1 base_manpower=1 culture = culture3 religion = anglican }\n";		// substate TA-9
	provinceStream << "-10={}\n";																																		// irrelevant
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.loadDefaultMapParser(defaults);
	provinceManager.loadDefinitionScraper(definitions);
	provinceManager.classifyProvinces(regionMapper);
	provinceManager.buildProvinceWeights();
	provinceManager.buildPopRatios({}, false);

	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings_chunks.txt");

	const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});

	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.loadTerrainsIntoProvinces(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();

	clayManager.generateChunks(provinceMapper, provinceManager);

	std::stringstream countryStream;
	const auto ta2 = std::make_shared<EU4::Country>("TA2", countryStream);
	const auto ta3 = std::make_shared<EU4::Country>("TA3", countryStream);
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	const std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", ta2}, {"TA3", ta3}, {"TA9", ta9}};
	clayManager.unDisputeChunkOwnership(countries);
	clayManager.splitChunksIntoSubStates();

	auto countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("TestFiles/configurables/country_mappings.txt");
	V3::PoliticalManager politicalManager;
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(countries);
	clayManager.assignSubStateOwnership(politicalManager.getCountries(), *countryMapper);

	return {clayManager, politicalManager};
}

std::tuple<V3::PopManager, V3::PoliticalManager, mappers::CultureMapper, mappers::ReligionMapper, V3::ClayManager, EU4::CultureLoader, EU4::ReligionLoader>
prepMappers()
{
	const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "TestFiles/mod/themod/"));
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	auto [clayManager, politicalManager] = prepManagers();
	mappers::ReligionMapper relMapper;
	relMapper.loadMappingRules("TestFiles/configurables/religion_map.txt");
	relMapper.expandReligionMappings(religionLoader.getAllReligions());

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");
	culMapper.expandCulturalMappings(clayManager, cultureLoader, religionLoader);

	V3::PopManager popManager;
	popManager.initializeVanillaPops(modFS);
	popManager.convertDemographics(clayManager, culMapper, relMapper, cultureLoader, religionLoader);
	clayManager.shoveRemainingProvincesIntoSubStates();
	politicalManager.generateDecentralizedCountries(clayManager, popManager);

	popManager.generatePops(clayManager);

	return std::tuple{popManager, politicalManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader};
}

} // namespace

TEST(V3World_EconomyManagerTests, TBD)
{
}