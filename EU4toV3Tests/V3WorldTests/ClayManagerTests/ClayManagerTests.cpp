#include "ClayManager/ClayManager.h"
#include "ClayManager/State/Chunk.h"
#include "ClayManager/State/Province.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "CountryManager/EU4Country.h"
#include "CountryMapper/CountryMapper.h"
#include "Loaders/SuperRegionLoader/V3Region.h"
#include "Loaders/SuperRegionLoader/V3SuperRegion.h"
#include "PoliticalManager/PoliticalManager.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

V3::ClayManager generateChunks()
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
	return clayManager;
}

V3::ClayManager distributeChunks()
{
	auto clayManager = generateChunks();
	std::stringstream countryStream;
	const auto ta2 = std::make_shared<EU4::Country>("TA2", countryStream);
	const auto ta3 = std::make_shared<EU4::Country>("TA3", countryStream);
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	const std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", ta2}, {"TA3", ta3}, {"TA9", ta9}};
	clayManager.unDisputeChunkOwnership(countries);
	clayManager.splitChunksIntoSubStates();
	return clayManager;
}

V3::ClayManager assignSubStateOwnership()
{
	auto clayManager = distributeChunks();
	std::stringstream countryStream;
	const auto ta2 = std::make_shared<EU4::Country>("TA2", countryStream);
	const auto ta3 = std::make_shared<EU4::Country>("TA3", countryStream);
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	const std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", ta2}, {"TA3", ta3}, {"TA9", ta9}};
	auto countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("TestFiles/configurables/country_mappings.txt");
	V3::PoliticalManager politicalManager;
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(countries);
	clayManager.assignSubStateOwnership(politicalManager.getCountries(), *countryMapper);

	return clayManager;
}

TEST(V3World_ClayManagerTests, clayManagerCanInitializeVanillaStates)
{
	V3::ClayManager clayManager;

	EXPECT_EQ(0, clayManager.getStates().size());

	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");

	EXPECT_EQ(10, clayManager.getStates().size());
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

	const auto& state1 = clayManager.getStates().at("STATE_TEST_LAKE");
	const auto& province1 = state1->getProvinces().at("xAABBCC");
	const auto& state2 = clayManager.getStates().at("STATE_TEST_2");
	const auto& province2 = state2->getProvinces().at("xDDEEFF");

	EXPECT_EQ("lakes", province1->getTerrain());
	EXPECT_EQ("ocean", province2->getTerrain());
}

TEST(V3World_ClayManagerTests, oceanProvincesGetFlaggedAsSeas)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.loadTerrainsIntoProvinces("TestFiles/vic3installation/game/");

	const auto& state1 = clayManager.getStates().at("STATE_TEST_LAKE");
	const auto& province1 = state1->getProvinces().at("xAABBCC");
	const auto& state2 = clayManager.getStates().at("STATE_TEST_2");
	const auto& province2 = state2->getProvinces().at("xDDEEFF");

	EXPECT_FALSE(province1->isSea());
	EXPECT_TRUE(province2->isSea());
}

TEST(V3World_ClayManagerTests, lakeProvincesGetFlaggedAsLakesandImpassables)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.loadTerrainsIntoProvinces("TestFiles/vic3installation/game/");

	const auto& state1 = clayManager.getStates().at("STATE_TEST_LAKE");
	const auto& province1 = state1->getProvinces().at("xAABBCC");
	const auto& state2 = clayManager.getStates().at("STATE_TEST_2");
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

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Terrain for province x345678 cannot be found.)"));

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

TEST(V3World_ClayManagerTests, clayManagerCanDetermineRegionValidity)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();

	EXPECT_TRUE(clayManager.regionIsValid("STATE_TEST_1"));					// state
	EXPECT_TRUE(clayManager.regionIsValid("region_a"));						// region
	EXPECT_TRUE(clayManager.regionIsValid("test_1_strategic_regions"));	// superregion
	EXPECT_FALSE(clayManager.regionIsValid("STATE_TEST_5"));					// does't exist
	EXPECT_FALSE(clayManager.regionIsValid("region_d"));						// does't exist
	EXPECT_FALSE(clayManager.regionIsValid("test_3_strategic_regions")); // does't exist
}

TEST(V3World_ClayManagerTests, clayManagerCanDetermineHierarchy)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();

	EXPECT_TRUE(clayManager.stateIsInRegion("STATE_TEST_1", "STATE_TEST_1"));
	EXPECT_TRUE(clayManager.stateIsInRegion("STATE_TEST_1", "region_a"));
	EXPECT_TRUE(clayManager.stateIsInRegion("STATE_TEST_1", "test_1_strategic_regions"));
	EXPECT_FALSE(clayManager.stateIsInRegion("STATE_TEST_1", "STATE_TEST_2"));
	EXPECT_FALSE(clayManager.stateIsInRegion("STATE_TEST_1", "region_b"));
	EXPECT_FALSE(clayManager.stateIsInRegion("STATE_TEST_1", "test_2_strategic_regions"));
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

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Attempting to assign state STATE_TEST_EXCESS which doesn't exist to region region_c!)"));

	std::cout.rdbuf(cout_buffer);

	const auto& region_c = clayManager.getSuperRegions().at("test_2_strategic_regions")->getRegions().at("region_c");

	EXPECT_FALSE(region_c->containsState("STATE_TEST_EXCESS"));
}

TEST(V3World_ClayManagerTests, clayManagerCanGenerateSaneChunks)
{
	auto clayManager = generateChunks();
	const auto& chunks = clayManager.getChunks();

	/*
	link = { eu4 = 1 vic3 = x000001 vic3 = x000002 } #sea->seas // produces chunk 1
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces chunk 2
	link = { eu4 = 4 vic3 = x000005 } #wasteland->land // produces wasteland-only chunk 3
	link = { eu4 = 5 vic3 = x000006 } #rnw->land // doesn't produce anything as rnw isn't valid source
	link = { eu4 = 6 eu4 = 7 vic3 = x000007 } #lake,land->lake // dropped because target lake is junk
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } #wasteland,land->land,lake // produces chunk 4, lake target dropped
	link = { eu4 = 10 } #land->nothing // produces nothing as there's no target.
	link = { vic3 = x000010 } #nothing->land // produces nothing as	there's no source.
	*/

	ASSERT_EQ(4, chunks.size());

	const auto& chunk1 = chunks[0]; // sea chunk 1->x1,x2
	const auto& chunk2 = chunks[1]; // land chunk, 2,3->x3,x4
	const auto& chunk3 = chunks[2]; // wasteland chunk, 4->x5
	const auto& chunk4 = chunks[3]; // land chunk, 9->x8

	EXPECT_TRUE(chunk1->getProvinces().contains("x000001"));
	EXPECT_TRUE(chunk1->getProvinces().contains("x000002"));
	EXPECT_EQ(2, chunk1->getProvinces().size());

	EXPECT_TRUE(chunk2->getProvinces().contains("x000003"));
	EXPECT_TRUE(chunk2->getProvinces().contains("x000004"));
	EXPECT_EQ(2, chunk2->getProvinces().size());

	EXPECT_TRUE(chunk3->getProvinces().contains("x000005"));
	EXPECT_EQ(1, chunk3->getProvinces().size());

	EXPECT_TRUE(chunk4->getProvinces().contains("x000008"));
	EXPECT_EQ(1, chunk4->getProvinces().size());
}

TEST(V3World_ClayManagerTests, clayManagerCanUndisputeChunkOwnership)
{
	auto clayManager = generateChunks();

	// Wee need some countries to link chunks to.
	std::stringstream countryStream;
	const auto tag = std::make_shared<EU4::Country>("TA2", countryStream);
	const auto gat = std::make_shared<EU4::Country>("TA9", countryStream);
	std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", tag}, {"TA9", gat}};

	clayManager.unDisputeChunkOwnership(countries);
	const auto& chunks = clayManager.getChunks();

	/*
	link = { eu4 = 1 vic3 = x000001 vic3 = x000002 } #sea->seas // produces chunk 1 - dropped since no owners
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces chunk 2 - goes to TA2 due to higher dev.
	link = { eu4 = 4 vic3 = x000005 } # wasteland -> land // produces chunk 3 - goes to noone since it's unowned.
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # wasteland,land->land,lake // produces chunk 4, goes to TA9 since only owner.
	*/

	ASSERT_EQ(3, chunks.size());

	const auto& chunk1 = chunks[0]; // land chunk, 2,3->x3,x4
	const auto& chunk2 = chunks[1]; // land chunk, 4->x5
	const auto& chunk3 = chunks[2]; // land chunk, 9->x8

	ASSERT_TRUE(chunk1->getSourceOwnerTag());
	EXPECT_EQ("TA2", chunk1->getSourceOwnerTag());
	EXPECT_TRUE(chunk1->getProvinces().contains("x000003"));
	EXPECT_TRUE(chunk1->getProvinces().contains("x000004"));

	EXPECT_FALSE(chunk2->getSourceOwnerTag());
	EXPECT_TRUE(chunk2->getProvinces().contains("x000005"));

	ASSERT_TRUE(chunk3->getSourceOwnerTag());
	EXPECT_EQ("TA9", chunk3->getSourceOwnerTag());
	EXPECT_TRUE(chunk3->getProvinces().contains("x000008"));
}

TEST(V3World_ClayManagerTests, clayManagerResetsChunkOwnershipFromInvalidCountries)
{
	auto clayManager = generateChunks();

	// Wee need some countries to link chunks to.
	std::stringstream countryStream;
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA9", ta9}};

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	clayManager.unDisputeChunkOwnership(countries);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Chunk owner TA2 is invalid. Dropping chunk ownership.)"));

	std::cout.rdbuf(cout_buffer);

	const auto& chunks = clayManager.getChunks();

	/*
	link = { eu4 = 1 vic3 = x000001 vic3 = x000002 } #sea->seas // produces chunk 1 - dropped since no owners
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces chunk 2 - goes to TA2 due to higher dev.
	link = { eu4 = 4 vic3 = x000005 } # wasteland -> land // produces chunk 3 - goes to noone since it's unowned.
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # wasteland,land->land,lake // produces chunk 4, goes to TA9 since only owner.
	*/

	ASSERT_EQ(3, chunks.size());

	const auto& chunk1 = chunks[0]; // land chunk, 2,3->x3,x4 // TA2 -> noone
	const auto& chunk2 = chunks[1]; // land chunk, 4->x5 // noone
	const auto& chunk3 = chunks[2]; // land chunk, 9->x8 // TA9

	EXPECT_FALSE(chunk1->getSourceOwnerTag());
	EXPECT_TRUE(chunk1->getProvinces().contains("x000003"));
	EXPECT_TRUE(chunk1->getProvinces().contains("x000004"));

	EXPECT_FALSE(chunk2->getSourceOwnerTag());
	EXPECT_TRUE(chunk2->getProvinces().contains("x000005"));

	ASSERT_TRUE(chunk3->getSourceOwnerTag());
	EXPECT_EQ("TA9", chunk3->getSourceOwnerTag());
	EXPECT_TRUE(chunk3->getProvinces().contains("x000008"));
}

TEST(V3World_ClayManagerTests, clayManagerResetsChunkOwnershipFromInsaneCountries)
{
	auto clayManager = generateChunks();

	// Wee need some countries to link chunks to.
	std::stringstream countryStream;
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", nullptr}, {"TA9", ta9}}; // TA2 is insane.

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	clayManager.unDisputeChunkOwnership(countries);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Chunk owner TA2 is not initialized. Dropping chunk ownership.)"));

	std::cout.rdbuf(cout_buffer);

	const auto& chunks = clayManager.getChunks();

	/*
	link = { eu4 = 1 vic3 = x000001 vic3 = x000002 } #sea->seas // produces chunk 1 - dropped since no owners
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces chunk 2 - goes to TA2 due to higher dev.
	link = { eu4 = 4 vic3 = x000005 } # wasteland -> land // produces chunk 3 - goes to noone since it's unowned.
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # wasteland,land->land,lake // produces chunk 4, goes to TA9 since only owner.
	*/

	ASSERT_EQ(3, chunks.size());

	const auto& chunk1 = chunks[0]; // land chunk, 2,3->x3,x4 // TA2 -> noone
	const auto& chunk2 = chunks[1]; // land chunk, 4->x5 // noone
	const auto& chunk3 = chunks[2]; // land chunk, 9->x8 // TA9

	EXPECT_FALSE(chunk1->getSourceOwnerTag());
	EXPECT_TRUE(chunk1->getProvinces().contains("x000003"));
	EXPECT_TRUE(chunk1->getProvinces().contains("x000004"));

	EXPECT_FALSE(chunk2->getSourceOwnerTag());
	EXPECT_TRUE(chunk2->getProvinces().contains("x000005"));

	ASSERT_TRUE(chunk3->getSourceOwnerTag());
	EXPECT_EQ("TA9", chunk3->getSourceOwnerTag());
	EXPECT_TRUE(chunk3->getProvinces().contains("x000008"));
}

TEST(V3World_ClayManagerTests, clayManagerCanProduceSubstatesFromChunks)
{
	auto clayManager = distributeChunks();
	const auto& substates = clayManager.getSubStates();

	/*
	link = { eu4 = 1 vic3 = x000001 vic3 = x000002 } #sea->seas // produces chunk 1 - dropped since no owners - no substate
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces substates 1 & 2 since x3 and x4 are in different states.
	link = { eu4 = 4 vic3 = x000005 } # wasteland -> land // produces chunk 3 // produces substate 3 which has no owner
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # lake,land->land,lake // produces substate 4, with only x8 inside as x9 is a lake.
	*/

	ASSERT_EQ(4, substates.size());

	const auto& substate1 = substates[0];
	const auto& substate2 = substates[1];
	const auto& substate3 = substates[2];
	const auto& substate4 = substates[3];

	ASSERT_TRUE(substate1->getSourceOwnerTag());
	EXPECT_EQ("TA2", *substate1->getSourceOwnerTag());
	EXPECT_EQ(1, substate1->getProvinces().size());
	EXPECT_TRUE(substate1->getProvinces().contains("x000003"));
	EXPECT_EQ("STATE_TEST_LAND1", substate1->getHomeStateName());

	ASSERT_TRUE(substate2->getSourceOwnerTag());
	EXPECT_EQ("TA2", *substate2->getSourceOwnerTag());
	EXPECT_EQ(1, substate2->getProvinces().size());
	EXPECT_TRUE(substate2->getProvinces().contains("x000004"));
	EXPECT_EQ("STATE_TEST_LAND2", substate2->getHomeStateName());

	ASSERT_FALSE(substate3->getSourceOwnerTag());
	EXPECT_EQ(1, substate3->getProvinces().size());
	EXPECT_TRUE(substate3->getProvinces().contains("x000005"));
	EXPECT_EQ("STATE_TEST_LAND3", substate3->getHomeStateName());

	ASSERT_TRUE(substate4->getSourceOwnerTag());
	EXPECT_EQ("TA9", *substate4->getSourceOwnerTag());
	EXPECT_EQ(1, substate4->getProvinces().size());
	EXPECT_TRUE(substate4->getProvinces().contains("x000008"));
	EXPECT_EQ("STATE_TEST_LAND4", substate4->getHomeStateName());
}

TEST(V3World_ClayManagerTests, clayManagerCanAssignSubStatesToCountries)
{
	auto clayManager = assignSubStateOwnership();
	const auto& substates = clayManager.getSubStates();

	/*
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces substates 1 & 2 for V3's GA2.
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # lake,land->land,lake // produces substate 4, for V3's GA9.
	*/

	ASSERT_EQ(3, substates.size());

	const auto& substate1 = substates[0];
	const auto& substate2 = substates[1];
	const auto& substate3 = substates[2];

	ASSERT_TRUE(substate1->getSourceOwnerTag());
	EXPECT_EQ("TA2", substate1->getSourceOwnerTag());
	EXPECT_EQ(1, substate1->getProvinces().size());
	EXPECT_TRUE(substate1->getProvinces().contains("x000003"));
	EXPECT_EQ("STATE_TEST_LAND1", substate1->getHomeStateName());
	EXPECT_EQ("GA2", substate1->getOwnerTag());
	EXPECT_EQ("GA2", substate1->getOwner()->getTag());
	// linkback to this substate through getOwner() substates vector
	EXPECT_EQ("STATE_TEST_LAND1", substate1->getOwner()->getSubStates()[0]->getHomeStateName());
	// linkback through state's substate ownership vector
	EXPECT_EQ("GA2", substate1->getHomeState()->getSubStates()[0]->getOwnerTag());

	ASSERT_TRUE(substate2->getSourceOwnerTag());
	EXPECT_EQ("TA2", substate2->getSourceOwnerTag());
	EXPECT_EQ(1, substate2->getProvinces().size());
	EXPECT_TRUE(substate2->getProvinces().contains("x000004"));
	EXPECT_EQ("STATE_TEST_LAND2", substate2->getHomeStateName());
	EXPECT_EQ("GA2", substate2->getOwnerTag());
	EXPECT_EQ("GA2", substate2->getOwner()->getTag());
	// linkback to this substate through getOwner() substates vector
	EXPECT_EQ("STATE_TEST_LAND2", substate2->getOwner()->getSubStates()[1]->getHomeStateName());
	// linkback through state's substate ownership vector
	EXPECT_EQ("GA2", substate2->getHomeState()->getSubStates()[0]->getOwnerTag());

	ASSERT_TRUE(substate3->getSourceOwnerTag());
	EXPECT_EQ("TA9", substate3->getSourceOwnerTag());
	EXPECT_EQ(1, substate3->getProvinces().size());
	EXPECT_TRUE(substate3->getProvinces().contains("x000008"));
	EXPECT_EQ("STATE_TEST_LAND4", substate3->getHomeStateName());
	EXPECT_EQ("GA9", substate3->getOwnerTag());
	EXPECT_EQ("GA9", substate3->getOwner()->getTag());
	// linkback to this substate through getOwner() substates vector
	EXPECT_EQ("STATE_TEST_LAND4", substate3->getOwner()->getSubStates()[0]->getHomeStateName());
	// linkback through state's substate ownership vector
	EXPECT_EQ("GA9", substate3->getHomeState()->getSubStates()[0]->getOwnerTag());
}
