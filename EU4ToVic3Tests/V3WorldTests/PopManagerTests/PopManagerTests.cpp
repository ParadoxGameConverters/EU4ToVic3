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
#include <gmock/gmock-matchers.h>

namespace
{
const auto eu4FS = commonItems::ModFilesystem("TestFiles/eu4installation/", {Mod("Some mod", "TestFiles/mod/themod/")});
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});

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

std::tuple<V3::PopManager, V3::PoliticalManager, mappers::CultureMapper, mappers::ReligionMapper, V3::ClayManager, EU4::CultureLoader, EU4::ReligionLoader>
prepMappers()
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

	return std::tuple{popManager, politicalManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader};
}
} // namespace

TEST(V3World_PopManagerTests, popManagerCanInitializeVanillaPopsAndPingThem)
{
	V3::PopManager popManager;

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	popManager.initializeVanillaPops(modFS);

	std::cout.rdbuf(cout_buffer);

	/*
	STATE_TEST_LAND1 - 600
	STATE_TEST_LAND2 - 3000
	STATE_TEST_LAND3 - 900
	STATE_TEST_LAND4 - 1000
	total: 5500
	*/

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([INFO] <> Vanilla had 5500 pops.)"));

	EXPECT_FALSE(popManager.getVanillaSubStatePops("STATE_NONSENSE", "AAA"));
	EXPECT_FALSE(popManager.getVanillaSubStatePops("STATE_TEST_LAND1", "NONSENSE"));
	ASSERT_TRUE(popManager.getVanillaSubStatePops("STATE_TEST_LAND1", "AAA"));

	const auto& subPops = popManager.getVanillaSubStatePops("STATE_TEST_LAND1", "AAA");
	EXPECT_EQ(600, subPops->getPopCount());
	EXPECT_EQ("ashkenazi", popManager.getDominantVanillaCulture("STATE_TEST_LAND1"));	// 300 pops
	EXPECT_EQ("noreligion", popManager.getDominantVanillaReligion("STATE_TEST_LAND1")); // none have set religion.
}

TEST(V3World_PopManagerTests, popManagerCanConvertDemographics)
{
	auto [popManager, politicalManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	ASSERT_EQ(7, clayManager.getSubStates().size());
	const auto& substate1 = clayManager.getSubStates()[0]; // 2 demos inside, from provinces 2 & 3
	ASSERT_EQ(2, substate1->getDemographics().size());
	const auto& s1demo1 = substate1->getDemographics()[0];
	const auto& s1demo2 = substate1->getDemographics()[1];
	EXPECT_EQ("vculture1", s1demo1.culture); // updated demos
	EXPECT_EQ("vculture2", s1demo2.culture); // updated demos
	EXPECT_EQ("catholic", s1demo1.religion);
	EXPECT_EQ("protestant", s1demo2.religion);
	EXPECT_NEAR(0.909, s1demo1.upperRatio, 0.001);
	EXPECT_NEAR(0.091, s1demo2.upperRatio, 0.001);

	const auto& substate2 = clayManager.getSubStates()[1]; // 2 demos inside, from provinces 2 & 3
	ASSERT_EQ(2, substate2->getDemographics().size());
	const auto& s2demo1 = substate2->getDemographics()[0];
	const auto& s2demo2 = substate2->getDemographics()[1];
	EXPECT_EQ("vculture1", s2demo1.culture); // updated demos
	EXPECT_EQ("vculture2", s2demo2.culture); // updated demos
	EXPECT_EQ("catholic", s2demo1.religion);
	EXPECT_EQ("protestant", s2demo2.religion);
	EXPECT_NEAR(0.909, s2demo1.upperRatio, 0.001);
	EXPECT_NEAR(0.091, s2demo2.upperRatio, 0.001);

	const auto& substate3 = clayManager.getSubStates()[2]; // single demo inside, from eu4 prov 9.
	ASSERT_EQ(1, substate3->getDemographics().size());
	const auto& s3demo = substate3->getDemographics()[0];
	EXPECT_EQ("vculture2", s3demo.culture);	// updated demos
	EXPECT_EQ("protestant", s3demo.religion); // updated demos
	EXPECT_EQ(1, s3demo.upperRatio);
}

TEST(V3World_PopManagerTests, popManagerCanGeneratePops)
{
	auto [popManager, politicalManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	popManager.generatePops(clayManager, Configuration::POPSHAPES::Vanilla, 1);


	/*
		STATE_TEST_LAND1 - 600 - goes to GA2 - split in 2 pops for its 2 demographics
		STATE_TEST_LAND2 - 3000 - goes to GA2 - split in 2 pops for its 2 demographics
		STATE_TEST_LAND3 - 900 - goes to shoved X02 substate with vanilla demo
		STATE_TEST_LAND4 - 1000 - goes to GA9 - single pop for single demo
		total: 5500
	*/

	const auto& pop1 = clayManager.getStates().at("STATE_TEST_LAND1")->getSubStates()[0]->getSubStatePops().getPops()[0];
	const auto& pop2 = clayManager.getStates().at("STATE_TEST_LAND1")->getSubStates()[0]->getSubStatePops().getPops()[1];
	EXPECT_EQ(pop1, V3::Pop("vculture1", "catholic", "", 0));
	EXPECT_EQ(545, pop1.getSize()); // 545 = 0.91 * 600
	EXPECT_EQ(pop2, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(55, pop2.getSize()); // 55 = 0.09 * 600

	const auto& pop3 = clayManager.getStates().at("STATE_TEST_LAND2")->getSubStates()[0]->getSubStatePops().getPops()[0];
	const auto& pop4 = clayManager.getStates().at("STATE_TEST_LAND2")->getSubStates()[0]->getSubStatePops().getPops()[1];
	EXPECT_EQ(pop3, V3::Pop("vculture1", "catholic", "", 0));
	EXPECT_EQ(2727, pop3.getSize()); // 2727 = 0.91 * 3000
	EXPECT_EQ(pop4, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(273, pop4.getSize()); // 273 = 0.09 * 3000

	const auto& pop5 = clayManager.getStates().at("STATE_TEST_LAND3")->getSubStates()[0]->getSubStatePops().getPops()[0];
	EXPECT_EQ(pop5, V3::Pop("swedish", "", "", 0)); // vanilla demo
	EXPECT_EQ(900, pop5.getSize());

	const auto& pop6 = clayManager.getStates().at("STATE_TEST_LAND4")->getSubStates()[0]->getSubStatePops().getPops()[0];
	EXPECT_EQ(pop6, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(1000, pop6.getSize());
}

TEST(V3World_PopManagerTests, popManagerCanGenerateVanillaPopsThroughShaping)
{
	auto [popManager, politicalManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	popManager.generatePops(clayManager, Configuration::POPSHAPES::PopShaping, 0);


	/*
		STATE_TEST_LAND1 - 600 - goes to GA2 - split in 2 pops for its 2 demographics
		STATE_TEST_LAND2 - 3000 - goes to GA2 - split in 2 pops for its 2 demographics
		STATE_TEST_LAND3 - 900 - goes to shoved X02 substate with vanilla demo
		STATE_TEST_LAND4 - 1000 - goes to GA9 - single pop for single demo
		total: 5500
	*/

	const auto& pop1 = clayManager.getStates().at("STATE_TEST_LAND1")->getSubStates()[0]->getSubStatePops().getPops()[0];
	const auto& pop2 = clayManager.getStates().at("STATE_TEST_LAND1")->getSubStates()[0]->getSubStatePops().getPops()[1];
	EXPECT_EQ(pop1, V3::Pop("vculture1", "catholic", "", 0));
	EXPECT_EQ(545, pop1.getSize()); // 545 = 0.91 * 600
	EXPECT_EQ(pop2, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(55, pop2.getSize()); // 55 = 0.09 * 600

	const auto& pop3 = clayManager.getStates().at("STATE_TEST_LAND2")->getSubStates()[0]->getSubStatePops().getPops()[0];
	const auto& pop4 = clayManager.getStates().at("STATE_TEST_LAND2")->getSubStates()[0]->getSubStatePops().getPops()[1];
	EXPECT_EQ(pop3, V3::Pop("vculture1", "catholic", "", 0));
	EXPECT_EQ(2727, pop3.getSize()); // 2727 = 0.91 * 3000
	EXPECT_EQ(pop4, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(273, pop4.getSize()); // 273 = 0.09 * 3000

	const auto& pop5 = clayManager.getStates().at("STATE_TEST_LAND3")->getSubStates()[0]->getSubStatePops().getPops()[0];
	EXPECT_EQ(pop5, V3::Pop("swedish", "", "", 0)); // vanilla demo
	EXPECT_EQ(900, pop5.getSize());

	const auto& pop6 = clayManager.getStates().at("STATE_TEST_LAND4")->getSubStates()[0]->getSubStatePops().getPops()[0];
	EXPECT_EQ(pop6, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(1000, pop6.getSize());
}

TEST(V3World_PopManagerTests, popManagerCanGenerateShapedPops)
{
	auto [popManager, politicalManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	popManager.generatePops(clayManager, Configuration::POPSHAPES::PopShaping, 1);

	/*
		STATE_TEST_LAND1 - 600 -> 622: projected popcount=725 * superRegionalNormalizationFactor=0.85649
		STATE_TEST_LAND2 - 3000->3108
		STATE_TEST_LAND3 - 900->900: vanilla state, no shaping
		STATE_TEST_LAND4 - 1000->1001
		total: 5500
	*/

	const auto& pop1 = clayManager.getStates().at("STATE_TEST_LAND1")->getSubStates()[0]->getSubStatePops().getPops()[0];
	const auto& pop2 = clayManager.getStates().at("STATE_TEST_LAND1")->getSubStates()[0]->getSubStatePops().getPops()[1];
	EXPECT_EQ(pop1, V3::Pop("vculture1", "catholic", "", 0));
	EXPECT_EQ(565, pop1.getSize()); // 565 = 0.91 * 622
	EXPECT_EQ(pop2, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(57, pop2.getSize()); // 57 = 0.091 * 622

	const auto& pop3 = clayManager.getStates().at("STATE_TEST_LAND2")->getSubStates()[0]->getSubStatePops().getPops()[0];
	const auto& pop4 = clayManager.getStates().at("STATE_TEST_LAND2")->getSubStates()[0]->getSubStatePops().getPops()[1];
	EXPECT_EQ(pop3, V3::Pop("vculture1", "catholic", "", 0));
	EXPECT_EQ(2825, pop3.getSize()); // 2825 = 0.91 * 3108
	EXPECT_EQ(pop4, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(283, pop4.getSize()); // 283 = 0.091 * 3108

	const auto& pop5 = clayManager.getStates().at("STATE_TEST_LAND3")->getSubStates()[0]->getSubStatePops().getPops()[0];
	EXPECT_EQ(pop5, V3::Pop("swedish", "", "", 0)); // vanilla demo
	EXPECT_EQ(900, pop5.getSize());

	const auto& pop6 = clayManager.getStates().at("STATE_TEST_LAND4")->getSubStates()[0]->getSubStatePops().getPops()[0];
	EXPECT_EQ(pop6, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(1001, pop6.getSize());
}

TEST(V3World_PopManagerTests, popManagerCanGenerateHomelands)
{
	auto [popManager, politicalManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader] = prepMappers();
	popManager.generatePops(clayManager, Configuration::POPSHAPES::Vanilla, 1);


	/*
		STATE_TEST_LAND1 - 600 - goes to GA2 - split in 2 pops for its 2 demographics
		STATE_TEST_LAND2 - 3000 - goes to GA2 - split in 2 pops for its 2 demographics
		STATE_TEST_LAND3 - 900 - goes to shoved X02 substate with vanilla demo
		STATE_TEST_LAND4 - 1000 - goes to GA9 - single pop for single demo
		total: 5500
	*/

	const auto& pop1 = clayManager.getStates().at("STATE_TEST_LAND1")->getSubStates()[0]->getSubStatePops().getPops()[0];
	const auto& pop2 = clayManager.getStates().at("STATE_TEST_LAND1")->getSubStates()[0]->getSubStatePops().getPops()[1];
	EXPECT_EQ(pop1, V3::Pop("vculture1", "catholic", "", 0));
	EXPECT_EQ(545, pop1.getSize()); // 545 = 0.91 * 600 <- primary culture
	EXPECT_EQ(pop2, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(55, pop2.getSize()); // 55 = 0.09 * 600

	const auto& pop3 = clayManager.getStates().at("STATE_TEST_LAND2")->getSubStates()[0]->getSubStatePops().getPops()[0];
	const auto& pop4 = clayManager.getStates().at("STATE_TEST_LAND2")->getSubStates()[0]->getSubStatePops().getPops()[1];
	EXPECT_EQ(pop3, V3::Pop("vculture1", "catholic", "", 0));
	EXPECT_EQ(2727, pop3.getSize()); // 2727 = 0.91 * 3000 <- primary culture
	EXPECT_EQ(pop4, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(273, pop4.getSize()); // 273 = 0.09 * 3000

	const auto& pop5 = clayManager.getStates().at("STATE_TEST_LAND3")->getSubStates()[0]->getSubStatePops().getPops()[0];
	EXPECT_EQ(pop5, V3::Pop("swedish", "", "", 0)); // vanilla demo
	EXPECT_EQ(900, pop5.getSize());						// <- primary culture

	const auto& pop6 = clayManager.getStates().at("STATE_TEST_LAND4")->getSubStates()[0]->getSubStatePops().getPops()[0];
	EXPECT_EQ(pop6, V3::Pop("vculture2", "protestant", "", 0));
	EXPECT_EQ(1000, pop6.getSize()); // <- primary culture

	popManager.applyHomeLands(clayManager);

	EXPECT_THAT(clayManager.getStates().at("STATE_TEST_LAND1")->getHomelands(), testing::UnorderedElementsAre("vculture1"));
	EXPECT_THAT(clayManager.getStates().at("STATE_TEST_LAND2")->getHomelands(), testing::UnorderedElementsAre("vculture1"));
	EXPECT_THAT(clayManager.getStates().at("STATE_TEST_LAND3")->getHomelands(), testing::UnorderedElementsAre("swedish"));
	EXPECT_THAT(clayManager.getStates().at("STATE_TEST_LAND4")->getHomelands(), testing::UnorderedElementsAre("vculture2"));
}