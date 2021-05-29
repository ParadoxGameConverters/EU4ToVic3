#include "Mappers/SuperGroupMapper/SuperRegionParser.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::ElementsAre;
using testing::Pair;
using testing::UnorderedElementsAre;

TEST(Mappers_SuperRegionParserTests, PrimitivesDefaultToEmpty)
{
	std::stringstream input;
	const auto superRegion = mappers::SuperRegionParser(input);

	EXPECT_TRUE(superRegion.getSuperRegionAssimilationMap().empty());
	EXPECT_TRUE(superRegion.getSuperRegionNames().empty());
}

TEST(Mappers_SuperRegionParserTests, SuperRegionCanBeLoaded)
{
	std::stringstream input;
	input << "sregion1 = { assimilation = 2 }\n";
	input << "sregion2 = { assimilation = 3 }\n";
	const auto superRegion = mappers::SuperRegionParser(input);

	EXPECT_THAT(superRegion.getSuperRegionNames(), UnorderedElementsAre("sregion1", "sregion2"));
	EXPECT_THAT(superRegion.getSuperRegionAssimilationMap(), ElementsAre(Pair("sregion1", 2), Pair("sregion2", 3)));
}
