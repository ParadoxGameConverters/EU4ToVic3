#include "Regions/SuperRegions.h"
#include "gtest/gtest.h"

TEST(EU4World_Regions_SuperRegions, blankSuperRegionLoadsWithNoRegions)
{
	std::stringstream input;
	const EU4::SuperRegions superRegion(input);

	ASSERT_TRUE(superRegion.getSuperRegions().empty());
}

TEST(EU4World_Regions_SuperRegions, superRegionsCanBeLoaded)
{
	std::stringstream input;
	input << "test_superregion = { region1 region2 }\n";
	input << "test2_superregion = { region3 region4 region5 }\n";
	const EU4::SuperRegions superRegion(input);

	ASSERT_EQ(2, superRegion.getSuperRegions().size());
	ASSERT_TRUE(superRegion.getSuperRegions().find("test_superregion")->second.contains("region1"));
	ASSERT_TRUE(superRegion.getSuperRegions().find("test_superregion")->second.contains("region2"));
	ASSERT_TRUE(superRegion.getSuperRegions().find("test2_superregion")->second.contains("region3"));
	ASSERT_TRUE(superRegion.getSuperRegions().find("test2_superregion")->second.contains("region4"));
	ASSERT_TRUE(superRegion.getSuperRegions().find("test2_superregion")->second.contains("region5"));
}

TEST(EU4World_Regions_SuperRegions, restrict_charterNonsenseIsIgnored)
{
	std::stringstream input;
	input << "test_superregion = {\n";
	input << "\trestrict_charter\n";
	input << "\tregion1 region2\n";
	input << "}\n";
	const EU4::SuperRegions superRegion(input);

	ASSERT_EQ(1, superRegion.getSuperRegions().size());
	ASSERT_TRUE(superRegion.getSuperRegions().find("test_superregion")->second.contains("region1"));
	ASSERT_TRUE(superRegion.getSuperRegions().find("test_superregion")->second.contains("region2"));
}
