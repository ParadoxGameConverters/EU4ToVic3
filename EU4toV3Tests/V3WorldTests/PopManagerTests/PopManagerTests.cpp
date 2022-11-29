#include "ClayManager/ClayManager.h"
#include "CountryMapper/CountryMapper.h"
#include "PoliticalManager/PoliticalManager.h"
#include "PopManager/Demographic.h"
#include "PopManager/PopManager.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

V3::ClayManager prepClayManager()
{
	auto eu4Path = "TestFiles/eu4installation/";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(eu4Path, {});
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(eu4Path, {});
	EU4::RegionManager regionMapper;
	regionMapper.loadRegions(eu4Path, {});

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";																																	  // sea, no ownership
	provinceStream << "-2={ owner = TA2 base_tax=10 base_production=10 base_manpower=10 culture = culture religion = religion }\n"; // substate TA-2&3
	provinceStream << "-3={ owner = TA3 base_tax=1 base_production=1 base_manpower=1 culture = culture2 religion = religion2 }\n";  // substate TA-2&3
	provinceStream << "-4={}\n";																																	  // irrelevant
	provinceStream << "-5={}\n";																																	  // irrelevant
	provinceStream << "-6={}\n";																																	  // irrelevant
	provinceStream << "-7={}\n";																																	  // irrelevant
	provinceStream << "-8={}\n";																																	  // irrelevant
	provinceStream << "-9={ owner = TA9 base_tax=1 base_production=1 base_manpower=1 culture = culture3 religion = religion3 }\n";  // substate TA-9
	provinceStream << "-10={}\n";																																	  // irrelevant
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.loadDefaultMapParser(defaults);
	provinceManager.loadDefinitionScraper(definitions);
	provinceManager.classifyProvinces(regionMapper);
	provinceManager.buildProvinceWeights();
	provinceManager.buildPopRatios({}, false);

	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings_chunks.txt");

	auto V3Path = "TestFiles/vic3installation/game/";

	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(V3Path);
	clayManager.loadTerrainsIntoProvinces(V3Path);
	clayManager.initializeSuperRegions(V3Path);
	clayManager.loadStatesIntoSuperRegions();

	clayManager.generateChunks(provinceMapper, provinceManager);

	std::stringstream countryStream;
	const auto ta2 = std::make_shared<EU4::Country>("TA2", countryStream);
	const auto ta3 = std::make_shared<EU4::Country>("TA3", countryStream);
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	const std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", ta2}, {"TA3", ta3}, {"TA9", ta9}};
	clayManager.unDisputeChunkOwnership(countries);
	clayManager.distributeChunksAcrossSubStates();

	auto countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("TestFiles/configurables/country_mappings.txt");
	V3::PoliticalManager politicalManager;
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(countries);
	clayManager.assignSubStateOwnership(politicalManager.getCountries(), *countryMapper);

	return clayManager;
}


TEST(V3World_PopManagerTests, popManagerCanInitializeVanillaPops)
{
	const auto V3Path = "TestFiles/vic3installation/game/";
	V3::PopManager popManager;

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	popManager.initializeVanillaPops(V3Path);

	std::cout.rdbuf(cout_buffer);

	/*
	STATE_TEST_1 - 600
	STATE_TEST_2 - 3000
	STATE_TEST_3 - 900
	STATE_TEST_4 - 1000
	total: 5500
	*/

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([INFO] <> Vanilla had 5500 pops.)"));
}

TEST(V3World_PopManagerTests, popManagerCanAssignVanillaPops)
{
	const auto clayManager = prepClayManager();
	V3::PopManager popManager;
	const auto V3Path = "TestFiles/vic3installation/game/";
	popManager.initializeVanillaPops(V3Path);
	popManager.assignVanillaPopsToStates(clayManager);

	const auto& state1 = clayManager.getStates().at("STATE_TEST_1");
	const auto& state2 = clayManager.getStates().at("STATE_TEST_2");
	const auto& state3 = clayManager.getStates().at("STATE_TEST_3");
	const auto& state4 = clayManager.getStates().at("STATE_TEST_4");

	EXPECT_EQ(600, state1->getVanillaPops().getPopCount());
	EXPECT_EQ(3000, state2->getVanillaPops().getPopCount());
	EXPECT_EQ(900, state3->getVanillaPops().getPopCount());
	EXPECT_EQ(1000, state4->getVanillaPops().getPopCount());
}

TEST(V3World_PopManagerTests, popManagerCanImportDemographics)
{
	const auto clayManager = prepClayManager();
	V3::PopManager popManager;
	popManager.importDemographics(clayManager);

	ASSERT_EQ(4, clayManager.getSubStates().size());
	const auto& substate1 = clayManager.getSubStates()[0]; // 2 demos inside, from provinces 2 & 3
	ASSERT_EQ(2, substate1->getDemographics().size());
	const auto& s1demo1 = substate1->getDemographics()[0];
	const auto& s1demo2 = substate1->getDemographics()[1];
	EXPECT_EQ("culture", s1demo1.culture);
	EXPECT_EQ("culture2", s1demo2.culture);
	EXPECT_EQ("religion", s1demo1.religion);
	EXPECT_EQ("religion2", s1demo2.religion);
	EXPECT_NEAR(0.909, s1demo1.upperRatio, 0.001); // 10-1 dev ratio
	EXPECT_NEAR(0.091, s1demo2.upperRatio, 0.001); // 1-10 dev ratio

	const auto& substate2 = clayManager.getSubStates()[1]; // 2 demos inside, from provinces 2 & 3
	ASSERT_EQ(2, substate2->getDemographics().size());
	const auto& s2demo1 = substate2->getDemographics()[0];
	const auto& s2demo2 = substate2->getDemographics()[1];
	EXPECT_EQ("culture", s2demo1.culture);
	EXPECT_EQ("culture2", s2demo2.culture);
	EXPECT_EQ("religion", s2demo1.religion);
	EXPECT_EQ("religion2", s2demo2.religion);
	EXPECT_NEAR(0.909, s2demo1.upperRatio, 0.001); // 10-1 dev ratio
	EXPECT_NEAR(0.091, s2demo2.upperRatio, 0.001); // 1-10 dev ratio

	const auto& substate3 = clayManager.getSubStates()[2]; // no demos inside, this is a wasteland->chunk substate
	EXPECT_TRUE(substate3->getDemographics().empty());

	const auto& substate4 = clayManager.getSubStates()[3]; // single demo inside, from eu4 prov 9.
	ASSERT_EQ(1, substate4->getDemographics().size());
	const auto& s4demo = substate4->getDemographics()[0];
	EXPECT_EQ("culture3", s4demo.culture);
	EXPECT_EQ("religion3", s4demo.religion);
	EXPECT_EQ(1, s4demo.upperRatio);
}
