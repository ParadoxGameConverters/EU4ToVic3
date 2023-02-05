#include <gmock/gmock-matchers.h>

#include "VNColonialMapper/VNColonialMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_VNColonialMapperTests, ColoniesDefaultToEmpty)
{
	std::stringstream input;
	mappers::VNColonialMapper mapper;
	mapper.loadMappingRules(input);

	EXPECT_TRUE(mapper.getVNColonies().empty());
}

TEST(Mappers_VNColonialMapperTests, ColoniesCanBeLoaded)
{
	std::stringstream input;
	input << R"(link = { name = "British Pacifics" key = x112233 owner = GBR states = { STATE_A STATE_B } decolonize = FRA })";
	input << R"(link = { name = "British SEA" key = x112233 owner = GBR states = { STATE_C STATE_D } })";
	mappers::VNColonialMapper mapper;
	mapper.loadMappingRules(input);

	EXPECT_EQ(2, mapper.getVNColonies().size());
	EXPECT_EQ("British Pacifics", mapper.getVNColonies()[0].getName());
	EXPECT_EQ("British SEA", mapper.getVNColonies()[1].getName());
}

TEST(Mappers_VNColonialMapperTests, StatesCanBePingedforColonialStatus)
{
	std::stringstream input;
	input << R"(link = { name = "British Pacifics" key = x112233 owner = GBR states = { STATE_A STATE_B } decolonize = FRA })";
	input << R"(link = { name = "British SEA" key = x112233 owner = GBR states = { STATE_C STATE_D } })";
	mappers::VNColonialMapper mapper;
	mapper.loadMappingRules(input);

	EXPECT_TRUE(mapper.isStateVNColonial("STATE_A"));
	EXPECT_FALSE(mapper.isStateVNColonial("STATE_E"));
}

TEST(Mappers_VNColonialMapperTests, ColonialOwnersCanBeReturned)
{
	std::stringstream input;
	input << R"(link = { name = "British Pacifics" key = x112233 owner = GBR states = { STATE_A STATE_B } decolonize = FRA })";
	input << R"(link = { name = "French PACIFICS" key = x332211 owner = FRA states = { STATE_A STATE_D } })";
	mappers::VNColonialMapper mapper;
	mapper.loadMappingRules(input);

	EXPECT_THAT(mapper.getVanillaOwners("STATE_A"), testing::UnorderedElementsAre("GBR", "FRA"));
	EXPECT_THAT(mapper.getVanillaOwners("STATE_B"), testing::UnorderedElementsAre("GBR"));
	EXPECT_THAT(mapper.getVanillaOwners("STATE_D"), testing::UnorderedElementsAre("FRA"));
}

TEST(Mappers_VNColonialMapperTests, KeyProvinceForStateOwnerMatchCanBeReturned)
{
	std::stringstream input;
	input << R"(link = { name = "British Pacifics" key = x112233 owner = GBR states = { STATE_A STATE_B } decolonize = FRA })";
	input << R"(link = { name = "French PACIFICS" key = x332211 owner = FRA states = { STATE_A STATE_D } })";
	mappers::VNColonialMapper mapper;
	mapper.loadMappingRules(input);

	EXPECT_EQ("x112233", *mapper.getKeyProvince("STATE_A", "GBR"));
	EXPECT_EQ("x332211", *mapper.getKeyProvince("STATE_A", "FRA"));
	EXPECT_FALSE(mapper.getKeyProvince("STATE_A", "NON"));
}

TEST(Mappers_VNColonialMapperTests, DecolonizableStatesCanBeMatchedForOwners)
{
	std::stringstream input;
	// always decolonize unless owner FRA
	input << R"(link = { name = "British Pacifics" key = x112233 owner = GBR states = { STATE_A STATE_B } decolonize = FRA })";
	// do not decolonize.
	input << R"(link = { name = "French PACIFICS" key = x332211 owner = FRA states = { STATE_A STATE_D } })";
	mappers::VNColonialMapper mapper;
	mapper.loadMappingRules(input);

	EXPECT_FALSE(mapper.isStateDecolonizable("STATE_A", "FRA", "GBR"));
	EXPECT_FALSE(mapper.isStateDecolonizable("STATE_A", "FRA", "FRA"));
	EXPECT_FALSE(mapper.isStateDecolonizable("STATE_A", "FRA", "NON"));

	EXPECT_TRUE(mapper.isStateDecolonizable("STATE_A", "GBR", "GBR"));
	EXPECT_FALSE(mapper.isStateDecolonizable("STATE_A", "GBR", "FRA"));
	EXPECT_TRUE(mapper.isStateDecolonizable("STATE_A", "GBR", "NON"));

	EXPECT_FALSE(mapper.isStateDecolonizable("STATE_A", "NON", "GBR"));
	EXPECT_FALSE(mapper.isStateDecolonizable("STATE_A", "NON", "FRA"));
	EXPECT_FALSE(mapper.isStateDecolonizable("STATE_A", "NON", "NON"));
}
