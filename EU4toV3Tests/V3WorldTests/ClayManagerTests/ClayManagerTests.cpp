#include "ClayManager/ClayManager.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

#include "Log.h"

TEST(V3World_ClayManagerTests, clayManagerCanInitializeVanillaStates)
{
	V3::ClayManager clayManager;

	EXPECT_EQ(0, clayManager.getStates().size());

	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");

	EXPECT_EQ(9, clayManager.getStates().size());
	EXPECT_TRUE(clayManager.getStates().contains("STATE_TEST_1"));
	EXPECT_TRUE(clayManager.getStates().contains("STATE_TEST_2"));
	EXPECT_TRUE(clayManager.getStates().contains("STATE_TEST_3"));
	EXPECT_TRUE(clayManager.getStates().contains("STATE_TEST_4"));
}

TEST(V3World_ClayManagerTests, clayManagerCanLoadTerrainsIntoStateProvinces)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.loadTerrainsIntoProvinces("TestFiles/vic3installation/game/");

	const auto& state2 = clayManager.getStates().at("STATE_TEST_2");
	const auto& province1 = state2->getProvinces().at("xAABBCC");
	const auto& province2 = state2->getProvinces().at("xDDEEFF");

	EXPECT_EQ("lakes", province1->getTerrain());
	EXPECT_EQ("ocean", province2->getTerrain());
}

TEST(V3World_ClayManagerTests, oceanProvincesGetFlaggedAsSeas)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.loadTerrainsIntoProvinces("TestFiles/vic3installation/game/");

	const auto& state2 = clayManager.getStates().at("STATE_TEST_2");
	const auto& province1 = state2->getProvinces().at("xAABBCC");
	const auto& province2 = state2->getProvinces().at("xDDEEFF");

	EXPECT_FALSE(province1->isSea());
	EXPECT_TRUE(province2->isSea());
}

TEST(V3World_ClayManagerTests, lakeProvincesGetFlaggedAsLakesandImpassables)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.loadTerrainsIntoProvinces("TestFiles/vic3installation/game/");

	const auto& state2 = clayManager.getStates().at("STATE_TEST_2");
	const auto& province1 = state2->getProvinces().at("xAABBCC");
	const auto& province2 = state2->getProvinces().at("xDDEEFF");

	EXPECT_TRUE(province1->isLake());
	EXPECT_TRUE(province1->isImpassable());
	EXPECT_FALSE(province2->isLake());
	EXPECT_FALSE(province2->isImpassable());
}

TEST(V3World_ClayManagerTests, clayManagerComplainsForMissingProvinceTerrain)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	clayManager.loadTerrainsIntoProvinces("TestFiles/vic3installation/game/");

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Terrain for province x345678 cannot be found.)"));

	std::cout.rdbuf(cout_buffer);
}


TEST(V3World_ClayManagerTests, clayManagerCanInitializeSuperRegions)
{
	V3::ClayManager clayManager;

	EXPECT_EQ(0, clayManager.getSuperRegions().size());

	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");

	EXPECT_EQ(2, clayManager.getSuperRegions().size());
	EXPECT_TRUE(clayManager.getSuperRegions().contains("test_1_strategic_regions"));
	EXPECT_TRUE(clayManager.getSuperRegions().contains("test_2_strategic_regions"));
}

TEST(V3World_ClayManagerTests, clayManagerCanLinkStatesToSuperRegions)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();

	const auto& superRegion1 = clayManager.getSuperRegions().at("test_1_strategic_regions");
	const auto& regionA = superRegion1->getRegions().at("region_a");
	const auto& states = regionA->getStates();

	EXPECT_EQ(2, states.size());

	const auto& state_test_1 = states.at("STATE_TEST_1");

	EXPECT_TRUE(state_test_1);
	EXPECT_EQ(3, state_test_1->getProvinces().size());
	EXPECT_TRUE(state_test_1->getProvinces().at("x445566")->isImpassable());
}

TEST(V3World_ClayManagerTests, excessSuperRegionStatesInRegionsAreObjectedAndRemoved)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	clayManager.loadStatesIntoSuperRegions();

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Attempting to assign state STATE_TEST_EXCESS which doesn't exist to region region_c!)"));

	std::cout.rdbuf(cout_buffer);

	const auto& region_c = clayManager.getSuperRegions().at("test_2_strategic_regions")->getRegions().at("region_c");

	EXPECT_FALSE(region_c->containsState("STATE_TEST_EXCESS"));
}

TEST(V3World_ClayManagerTests, clayManagerCanGenerateSaneChunks)
{
	auto eu4Path = "TestFiles/eu4installation/";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(eu4Path, {});
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(eu4Path, {});
	EU4::RegionManager regionMapper;
	regionMapper.loadRegions(eu4Path, {});

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";
	provinceStream << "-2={}\n";
	provinceStream << "-3={}\n";
	provinceStream << "-4={}\n";
	provinceStream << "-5={}\n";
	provinceStream << "-6={}\n";
	provinceStream << "-7={}\n";
	provinceStream << "-8={}\n";
	provinceStream << "-9={}\n";
	provinceStream << "-10={}\n";
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.loadDefaultMapParser(defaults);
	provinceManager.loadDefinitionScraper(definitions);
	provinceManager.classifyProvinces(regionMapper);

	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings_chunks.txt");

	auto V3Path = "TestFiles/vic3installation/game/";

	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(V3Path);
	clayManager.loadTerrainsIntoProvinces(V3Path);
	clayManager.initializeSuperRegions(V3Path);
	clayManager.loadStatesIntoSuperRegions();

	clayManager.generateChunks(provinceMapper, provinceManager);

	const auto& chunks = clayManager.getChunks();

	/*
	link = { eu4 = 1 vic3 = x000001 vic3 = x000002 } #sea->seas // produces chunk 1
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces chunk 2
	link = { eu4 = 4 vic3 = x000005 } #wasteland->land // doesn't produce anything as wasteland isn't valid source
	link = { eu4 = 5 vic3 = x000006 } #rnw->land // ditto, rnw isn't valid source
	link = { eu4 = 6 eu4 = 7 vic3 = x000007 } # lake,land->lake // dropped because target lake is junk
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # lake,land->land,lake // produces chunk 3, lake source is dropped as not viable, lake target also dropped.
	link = { eu4 = 10 } # land->nothing // produces nothing as there's no target.
	link = { vic3 = x000010 } # nothing->land // produces nothing as there's no source.
	*/

	EXPECT_EQ(3, chunks.size());

	const auto& chunk1 = chunks[0]; // sea chunk 1->x1,x2
	const auto& chunk2 = chunks[1]; // land chunk, 2,3->x3,x4 
	const auto& chunk3 = chunks[2]; // land chunk, 9->x8

	EXPECT_TRUE(chunk1->sourceProvinces.contains(1));
	EXPECT_EQ(1, chunk1->sourceProvinces.size());
	EXPECT_TRUE(chunk1->provinces.contains("x000001"));
	EXPECT_TRUE(chunk1->provinces.contains("x000002"));
	EXPECT_EQ(2, chunk1->provinces.size());
	EXPECT_TRUE(chunk1->states.contains("STATE_TEST_OCEAN1"));
	EXPECT_EQ(1, chunk1->states.size());

	EXPECT_TRUE(chunk2->sourceProvinces.contains(2));
	EXPECT_TRUE(chunk2->sourceProvinces.contains(3));
	EXPECT_EQ(2, chunk2->sourceProvinces.size());
	EXPECT_TRUE(chunk2->provinces.contains("x000003"));
	EXPECT_TRUE(chunk2->provinces.contains("x000004"));
	EXPECT_EQ(2, chunk2->provinces.size());
	EXPECT_TRUE(chunk2->states.contains("STATE_TEST_LAND1"));
	EXPECT_TRUE(chunk2->states.contains("STATE_TEST_LAND2"));
	EXPECT_EQ(2, chunk2->states.size());

	EXPECT_TRUE(chunk3->sourceProvinces.contains(9));
	EXPECT_EQ(1, chunk3->sourceProvinces.size());
	EXPECT_TRUE(chunk3->provinces.contains("x000008"));
	EXPECT_EQ(1, chunk3->provinces.size());
	EXPECT_TRUE(chunk3->states.contains("STATE_TEST_LAND4"));
	EXPECT_EQ(1, chunk3->states.size());
}
