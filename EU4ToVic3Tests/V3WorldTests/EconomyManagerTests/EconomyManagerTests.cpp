#include "ClayManager/ClayManager.h"
#include "ClayManager/State/SubState.h"
#include "CountryManager/EU4Country.h"
#include "CountryMapper/CountryMapper.h"
#include "CultureLoader/CultureLoader.h"
#include "CultureMapper/CultureMapper.h"
#include "EconomyManager/EconomyManager.h"
#include "PoliticalManager/Country/Country.h"
#include "PopManager/PopManager.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "ReligionLoader/ReligionLoader.h"
#include "ReligionMapper/ReligionMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>


namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
const auto eu4FS = commonItems::ModFilesystem("TestFiles/eu4installation/", {Mod("Some mod", "TestFiles/mod/themod/")});

std::tuple<V3::ClayManager, V3::PoliticalManager> prepManagers()
{
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(eu4FS);
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(eu4FS);
	EU4::RegionManager regionManager;
	regionManager.loadRegions(eu4FS);

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
	provinceManager.classifyProvinces(regionManager);
	provinceManager.buildProvinceWeights(regionManager);
	provinceManager.buildPopRatios({}, false);

	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings_chunks.txt");

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

V3::PoliticalManager prepWorld()
{
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4FS);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4FS);

	auto [clayManager, politicalManager] = prepManagers();
	mappers::ReligionMapper relMapper;
	relMapper.loadMappingRules("TestFiles/configurables/religion_map.txt");
	relMapper.expandReligionMappings(religionLoader.getAllReligions());

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");
	culMapper.expandCulturalMappings(clayManager, cultureLoader, religionLoader);

	V3::PopManager popManager;
	popManager.initializeVanillaPops(modFS);
	popManager.initializeDWPops(modFS);
	popManager.convertDemographics(clayManager, culMapper, relMapper, cultureLoader, religionLoader);
	clayManager.shoveRemainingProvincesIntoSubStates();
	politicalManager.generateDecentralizedCountries(clayManager, popManager);

	popManager.generatePops(clayManager, Configuration::POPSHAPES::Vanilla, 1);

	return politicalManager;
}

} // namespace

TEST(V3World_EconomyManagerTests, EconomyManagerStoresOnlyCentralizedStates)
{
	V3::EconomyManager econManager;

	auto politicalManager = prepWorld();

	EXPECT_EQ(0, econManager.getCentralizedCountries().size());
	EXPECT_EQ(5, politicalManager.getCountries().size());

	econManager.loadCentralizedStates(politicalManager.getCountries());

	EXPECT_EQ(2, econManager.getCentralizedCountries().size());
	EXPECT_EQ("GA2", econManager.getCentralizedCountries()[0]->getTag());
	EXPECT_EQ("GA9", econManager.getCentralizedCountries()[1]->getTag());
}

TEST(V3World_EconomyManagerTests, GlobalCPScalesByPopulation)
{
	auto politicalManager = prepWorld();

	V3::EconomyManager econManager;
	econManager.loadCentralizedStates(politicalManager.getCountries());
	econManager.loadMappersAndConfigs(modFS, "TestFiles/");

	EXPECT_EQ(5500, politicalManager.getWorldPopCount());
	EXPECT_EQ(4600, V3::PoliticalManager::getCountriesPopCount(econManager.getCentralizedCountries()));

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	econManager.assignCountryCPBudgets(Configuration::ECONOMY::Test, Configuration::STARTDATE::Vanilla, DatingData(), politicalManager);

	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([INFO] <> The world is 84% Centralized by population. Adjusting global CP values by: -14%)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"([INFO] <> The world has 1245324 CP to spend on industry.)"));
}