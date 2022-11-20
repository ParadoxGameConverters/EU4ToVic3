#include "ClayManager/ClayManager.h"
#include "CountryMapper/CountryMapper.h"
#include "PoliticalManager/PoliticalManager.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

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
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # lake,land->land,lake // produces chunk 3, lake source is dropped as not viable, lake target also
	dropped. link = { eu4 = 10 } # land->nothing // produces nothing as there's no target. link = { vic3 = x000010 } # nothing->land // produces nothing as
	there's no source.
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

TEST(V3World_ClayManagerTests, clayManagerCanUndisputeChunkOwnership)
{
	auto eu4Path = "TestFiles/eu4installation/";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(eu4Path, {});
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(eu4Path, {});
	EU4::RegionManager regionMapper;
	regionMapper.loadRegions(eu4Path, {});

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";																					// sea, no ownership
	provinceStream << "-2={ owner = TAG base_tax=10 base_production=10 base_manpower=10 }\n"; // dev 30
	provinceStream << "-3={ owner = GAT base_tax=1 base_production=1 base_manpower=1 }\n";		// dev 3
	provinceStream << "-4={}\n";																					// irrelevant
	provinceStream << "-5={}\n";																					// irrelevant
	provinceStream << "-6={}\n";																					// irrelevant
	provinceStream << "-7={}\n";																					// irrelevant
	provinceStream << "-8={}\n";																					// irrelevant
	provinceStream << "-9={ owner = GAT base_tax=1 base_production=1 base_manpower=1 }\n";		// dev 3
	provinceStream << "-10={}\n";																					// irrelevant
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.loadDefaultMapParser(defaults);
	provinceManager.loadDefinitionScraper(definitions);
	provinceManager.classifyProvinces(regionMapper);
	provinceManager.buildProvinceWeights();
	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings_chunks.txt");
	auto V3Path = "TestFiles/vic3installation/game/";
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(V3Path);
	clayManager.loadTerrainsIntoProvinces(V3Path);
	clayManager.initializeSuperRegions(V3Path);
	clayManager.loadStatesIntoSuperRegions();
	clayManager.generateChunks(provinceMapper, provinceManager);

	// Wee need some countries to link chunks to.
	std::stringstream countryStream;
	const auto tag = std::make_shared<EU4::Country>("TAG", countryStream);
	const auto gat = std::make_shared<EU4::Country>("GAT", countryStream);
	std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TAG", tag}, {"GAT", gat}};

	clayManager.unDisputeChunkOwnership(countries);
	const auto& chunks = clayManager.getChunks();

	/*
	link = { eu4 = 1 vic3 = x000001 vic3 = x000002 } #sea->seas // produces chunk 1 - dropped since no owners
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces chunk 2 - goes to TAG due to higher dev.
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # lake,land->land,lake // produces chunk 3, goes to GAT since only owner.
	*/

	EXPECT_EQ(2, chunks.size());

	const auto& chunk1 = chunks[0]; // land chunk, 2,3->x3,x4
	const auto& chunk2 = chunks[1]; // land chunk, 9->x8

	EXPECT_TRUE(chunk1->sourceOwner);
	EXPECT_EQ("TAG", chunk1->sourceOwner->getTag());
	EXPECT_TRUE(chunk1->sourceProvinces.contains(2));
	EXPECT_TRUE(chunk1->sourceProvinces.contains(3));
	EXPECT_TRUE(chunk1->provinces.contains("x000003"));
	EXPECT_TRUE(chunk1->provinces.contains("x000004"));

	EXPECT_TRUE(chunk2->sourceOwner);
	EXPECT_EQ("GAT", chunk2->sourceOwner->getTag());
	EXPECT_TRUE(chunk2->sourceProvinces.contains(9));
	EXPECT_TRUE(chunk2->provinces.contains("x000008"));
}

TEST(V3World_ClayManagerTests, clayManagerDropsChunksBelongingToInvalidCountries)
{
	auto eu4Path = "TestFiles/eu4installation/";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(eu4Path, {});
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(eu4Path, {});
	EU4::RegionManager regionMapper;
	regionMapper.loadRegions(eu4Path, {});

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";																					// sea, no ownership
	provinceStream << "-2={ owner = TAG base_tax=10 base_production=10 base_manpower=10 }\n"; // dev 30
	provinceStream << "-3={ owner = GAT base_tax=1 base_production=1 base_manpower=1 }\n";		// dev 3
	provinceStream << "-4={}\n";																					// irrelevant
	provinceStream << "-5={}\n";																					// irrelevant
	provinceStream << "-6={}\n";																					// irrelevant
	provinceStream << "-7={}\n";																					// irrelevant
	provinceStream << "-8={}\n";																					// irrelevant
	provinceStream << "-9={ owner = GAT base_tax=1 base_production=1 base_manpower=1 }\n";		// dev 3
	provinceStream << "-10={}\n";																					// irrelevant
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.loadDefaultMapParser(defaults);
	provinceManager.loadDefinitionScraper(definitions);
	provinceManager.classifyProvinces(regionMapper);
	provinceManager.buildProvinceWeights();
	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings_chunks.txt");
	auto V3Path = "TestFiles/vic3installation/game/";
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(V3Path);
	clayManager.loadTerrainsIntoProvinces(V3Path);
	clayManager.initializeSuperRegions(V3Path);
	clayManager.loadStatesIntoSuperRegions();
	clayManager.generateChunks(provinceMapper, provinceManager);

	// Wee need some countries to link chunks to.
	std::stringstream countryStream;
	const auto gat = std::make_shared<EU4::Country>("GAT", countryStream);
	std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"GAT", gat}};

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	clayManager.unDisputeChunkOwnership(countries);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Chunk owner TAG is invalid. Dropping chunk.)"));

	std::cout.rdbuf(cout_buffer);

	const auto& chunks = clayManager.getChunks();

	/*
	link = { eu4 = 1 vic3 = x000001 vic3 = x000002 } #sea->seas // produces chunk 1 - dropped since no owners
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces chunk 2 - goes to TAG due to higher dev, but there's no TAG so dropped
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # lake,land->land,lake // produces chunk 3, goes to GAT since only owner.
	*/

	EXPECT_EQ(1, chunks.size());

	const auto& chunk1 = chunks[0]; // land chunk, 9->x8

	EXPECT_TRUE(chunk1->sourceOwner);
	EXPECT_EQ("GAT", chunk1->sourceOwner->getTag());
	EXPECT_TRUE(chunk1->sourceProvinces.contains(9));
	EXPECT_TRUE(chunk1->provinces.contains("x000008"));
}

TEST(V3World_ClayManagerTests, clayManagerDropsChunksBelongingToInsaneCountries)
{
	auto eu4Path = "TestFiles/eu4installation/";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(eu4Path, {});
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(eu4Path, {});
	EU4::RegionManager regionMapper;
	regionMapper.loadRegions(eu4Path, {});

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";																					// sea, no ownership
	provinceStream << "-2={ owner = TAG base_tax=10 base_production=10 base_manpower=10 }\n"; // dev 30
	provinceStream << "-3={ owner = GAT base_tax=1 base_production=1 base_manpower=1 }\n";		// dev 3
	provinceStream << "-4={}\n";																					// irrelevant
	provinceStream << "-5={}\n";																					// irrelevant
	provinceStream << "-6={}\n";																					// irrelevant
	provinceStream << "-7={}\n";																					// irrelevant
	provinceStream << "-8={}\n";																					// irrelevant
	provinceStream << "-9={ owner = GAT base_tax=1 base_production=1 base_manpower=1 }\n";		// dev 3
	provinceStream << "-10={}\n";																					// irrelevant
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.loadDefaultMapParser(defaults);
	provinceManager.loadDefinitionScraper(definitions);
	provinceManager.classifyProvinces(regionMapper);
	provinceManager.buildProvinceWeights();
	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings_chunks.txt");
	auto V3Path = "TestFiles/vic3installation/game/";
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(V3Path);
	clayManager.loadTerrainsIntoProvinces(V3Path);
	clayManager.initializeSuperRegions(V3Path);
	clayManager.loadStatesIntoSuperRegions();
	clayManager.generateChunks(provinceMapper, provinceManager);

	// Wee need some countries to link chunks to.
	std::stringstream countryStream;
	const auto gat = std::make_shared<EU4::Country>("GAT", countryStream);
	std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TAG", nullptr}, {"GAT", gat}}; // TAG is insane.

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	clayManager.unDisputeChunkOwnership(countries);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Chunk owner TAG is not initialized. Dropping chunk.)"));

	std::cout.rdbuf(cout_buffer);

	const auto& chunks = clayManager.getChunks();

	/*
	link = { eu4 = 1 vic3 = x000001 vic3 = x000002 } #sea->seas // produces chunk 1 - dropped since no owners
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces chunk 2 - goes to TAG due to higher dev, but TAG is insane so dropped
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # lake,land->land,lake // produces chunk 3, goes to GAT since only owner.
	*/

	EXPECT_EQ(1, chunks.size());

	const auto& chunk1 = chunks[0]; // land chunk, 9->x8

	EXPECT_TRUE(chunk1->sourceOwner);
	EXPECT_EQ("GAT", chunk1->sourceOwner->getTag());
	EXPECT_TRUE(chunk1->sourceProvinces.contains(9));
	EXPECT_TRUE(chunk1->provinces.contains("x000008"));
}

TEST(V3World_ClayManagerTests, clayManagerCanProduceSubstatesFromChunks)
{
	auto eu4Path = "TestFiles/eu4installation/";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(eu4Path, {});
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(eu4Path, {});
	EU4::RegionManager regionMapper;
	regionMapper.loadRegions(eu4Path, {});

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";																					// sea, no ownership
	provinceStream << "-2={ owner = TAG base_tax=10 base_production=10 base_manpower=10 }\n"; // substate TAG-1&2
	provinceStream << "-3={ owner = GAT base_tax=1 base_production=1 base_manpower=1 }\n";		// substate TAG-1&2
	provinceStream << "-4={}\n";																					// irrelevant
	provinceStream << "-5={}\n";																					// irrelevant
	provinceStream << "-6={}\n";																					// irrelevant
	provinceStream << "-7={}\n";																					// irrelevant
	provinceStream << "-8={}\n";																					// irrelevant
	provinceStream << "-9={ owner = GAT base_tax=1 base_production=1 base_manpower=1 }\n";		// substate TAG-3
	provinceStream << "-10={}\n";																					// irrelevant
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.loadDefaultMapParser(defaults);
	provinceManager.loadDefinitionScraper(definitions);
	provinceManager.classifyProvinces(regionMapper);
	provinceManager.buildProvinceWeights();
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
	const auto tag = std::make_shared<EU4::Country>("TAG", countryStream);
	const auto gat = std::make_shared<EU4::Country>("GAT", countryStream);
	std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TAG", tag}, {"GAT", gat}};
	clayManager.unDisputeChunkOwnership(countries);

	clayManager.distributeChunksAcrossSubStates();

	const auto& substates = clayManager.getSubStates();

	/*
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces substates 1 & 2 since x3 and x4 are in different states.
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # lake,land->land,lake // produces substate 3, with only x8 inside as x9 is a lake.
	*/

	EXPECT_EQ(3, substates.size());

	const auto& substate1 = substates[0];
	const auto& substate2 = substates[1];
	const auto& substate3 = substates[2];

	EXPECT_TRUE(substate1->sourceOwner);
	EXPECT_EQ("GAT", substate1->sourceOwner->getTag());
	EXPECT_EQ(1, substate1->provinces.size());
	EXPECT_TRUE(substate1->provinces.contains("x000008"));
	EXPECT_EQ("STATE_TEST_LAND4", substate1->stateName);

	EXPECT_TRUE(substate2->sourceOwner);
	EXPECT_EQ("TAG", substate2->sourceOwner->getTag());
	EXPECT_EQ(1, substate2->provinces.size());
	EXPECT_TRUE(substate2->provinces.contains("x000003"));
	EXPECT_EQ("STATE_TEST_LAND1", substate2->stateName);

	EXPECT_TRUE(substate3->sourceOwner);
	EXPECT_EQ("TAG", substate3->sourceOwner->getTag());
	EXPECT_EQ(1, substate3->provinces.size());
	EXPECT_TRUE(substate3->provinces.contains("x000004"));
	EXPECT_EQ("STATE_TEST_LAND2", substate3->stateName);
}

TEST(V3World_ClayManagerTests, clayManagerCanAssignSubStatesToCountries)
{
	auto eu4Path = "TestFiles/eu4installation/";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(eu4Path, {});
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(eu4Path, {});
	EU4::RegionManager regionMapper;
	regionMapper.loadRegions(eu4Path, {});

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";																					// sea, no ownership
	provinceStream << "-2={ owner = TA2 base_tax=10 base_production=10 base_manpower=10 }\n"; // substate TA-2&3
	provinceStream << "-3={ owner = TA3 base_tax=1 base_production=1 base_manpower=1 }\n";		// substate TA-2&3
	provinceStream << "-4={}\n";																					// irrelevant
	provinceStream << "-5={}\n";																					// irrelevant
	provinceStream << "-6={}\n";																					// irrelevant
	provinceStream << "-7={}\n";																					// irrelevant
	provinceStream << "-8={}\n";																					// irrelevant
	provinceStream << "-9={ owner = TA9 base_tax=1 base_production=1 base_manpower=1 }\n";		// substate TA-9
	provinceStream << "-10={}\n";																					// irrelevant
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.loadDefaultMapParser(defaults);
	provinceManager.loadDefinitionScraper(definitions);
	provinceManager.classifyProvinces(regionMapper);
	provinceManager.buildProvinceWeights();
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
	std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", ta2}, {"TA3", ta3}, {"TA9", ta9}};
	clayManager.unDisputeChunkOwnership(countries);
	clayManager.distributeChunksAcrossSubStates();
	auto countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("TestFiles/configurables/country_mappings.txt");
	V3::PoliticalManager politicalManager;
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(countries);
	clayManager.assignSubStateOwnership(politicalManager.getCountries(), *countryMapper);

	const auto& substates = clayManager.getSubStates();

	/*
	link = { eu4 = 2 eu4 = 3 vic3 = x000003 vic3 = x000004 } #lands->lands // produces substates 1 & 2 for V3's GA2.
	link = { eu4 = 8 eu4 = 9 vic3 = x000008 vic3 = x000009 } # lake,land->land,lake // produces substate 3, for V3's GA9.
	*/

	EXPECT_EQ(3, substates.size());

	const auto& substate1 = substates[0];
	const auto& substate2 = substates[1];
	const auto& substate3 = substates[2];

	EXPECT_TRUE(substate1->sourceOwner);
	EXPECT_EQ("TA2", substate1->sourceOwner->getTag());
	EXPECT_EQ(1, substate1->provinces.size());
	EXPECT_TRUE(substate1->provinces.contains("x000003"));
	EXPECT_EQ("STATE_TEST_LAND1", substate1->stateName);
	EXPECT_EQ("GA2", substate1->ownerTag);
	EXPECT_EQ("GA2", substate1->owner->getTag());
	// linkback to this substate through owner substates vector
	EXPECT_EQ("STATE_TEST_LAND1", substate1->owner->getSubStates()[0]->stateName);

	EXPECT_TRUE(substate2->sourceOwner);
	EXPECT_EQ("TA2", substate2->sourceOwner->getTag());
	EXPECT_EQ(1, substate2->provinces.size());
	EXPECT_TRUE(substate2->provinces.contains("x000004"));
	EXPECT_EQ("STATE_TEST_LAND2", substate2->stateName);
	EXPECT_EQ("GA2", substate2->ownerTag);
	EXPECT_EQ("GA2", substate2->owner->getTag());
	// linkback to this substate through owner substates vector
	EXPECT_EQ("STATE_TEST_LAND2", substate2->owner->getSubStates()[1]->stateName);

	EXPECT_TRUE(substate3->sourceOwner);
	EXPECT_EQ("TA9", substate3->sourceOwner->getTag());
	EXPECT_EQ(1, substate3->provinces.size());
	EXPECT_TRUE(substate3->provinces.contains("x000008"));
	EXPECT_EQ("STATE_TEST_LAND4", substate3->stateName);
	EXPECT_EQ("GA9", substate3->ownerTag);
	EXPECT_EQ("GA9", substate3->owner->getTag());
	// linkback to this substate through owner substates vector
	EXPECT_EQ("STATE_TEST_LAND4", substate3->owner->getSubStates()[0]->stateName);
}
