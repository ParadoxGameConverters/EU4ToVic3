#include "ClayManager/ClayManager.h"
#include "ColonialRegionMapper/ColonialRegionMapper.h"
#include "gtest/gtest.h"

namespace
{
std::tuple<V3::ClayManager, mappers::ColonialRegionMapper> prepMappers()
{
	const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.loadTerrainsIntoProvinces(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();

	mappers::ColonialRegionMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/colonial_regions.txt");

	return {clayManager, mapper};
}
} // namespace

TEST(Mappers_ColonialRegionMapperTests, rulesCanBeLoadedInOrder)
{
	mappers::ColonialRegionMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/colonial_regions.txt");

	ASSERT_EQ(2, mapper.getColonialRegions().size());
	EXPECT_TRUE(mapper.getColonialRegions().contains("usa_north_colony"));
	EXPECT_TRUE(mapper.getColonialRegions().contains("usa_south_colony"));
}

TEST(Mappers_ColonialRegionMapperTests, coloniesCanBeMatchedViaDirectState)
{
	auto [clayMapper, mapper] = prepMappers();

	const auto& match1 = mapper.getColonyNameForState("STATE_TEST_4", clayMapper);
	const auto& match2 = mapper.getColonyForState("STATE_TEST_4", clayMapper);
	ASSERT_TRUE(match1);
	ASSERT_TRUE(match2);
	EXPECT_EQ("usa_south_colony", *match1);
	EXPECT_EQ("Floridian", match2->getAloneName());
}

TEST(Mappers_ColonialRegionMapperTests, coloniesCanBeMatchedViaRegion)
{
	auto [clayMapper, mapper] = prepMappers();

	const auto& match1 = mapper.getColonyNameForState("STATE_TEST_1", clayMapper);
	const auto& match2 = mapper.getColonyForState("STATE_TEST_1", clayMapper);
	ASSERT_TRUE(match1);
	ASSERT_TRUE(match2);
	EXPECT_EQ("usa_north_colony", *match1);
	EXPECT_EQ("Appalachian", match2->getAloneName());
}

TEST(Mappers_ColonialRegionMapperTests, mismapReturnsNullopt)
{
	auto [clayMapper, mapper] = prepMappers();

	const auto& match1 = mapper.getColonyNameForState("non", clayMapper);
	const auto& match2 = mapper.getColonyForState("non", clayMapper);
	ASSERT_FALSE(match1);
	ASSERT_FALSE(match2);
}
