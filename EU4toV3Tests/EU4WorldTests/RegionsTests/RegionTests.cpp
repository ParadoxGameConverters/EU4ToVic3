#include "Regions/Area.h"
#include "Regions/Region.h"
#include "gtest/gtest.h"

TEST(EU4World_Regions_RegionTests, oldStyleLoadViaProvincesDefaultsToEmpty)
{
	const std::set<int> input;
	const EU4::Region theRegion(input);

	ASSERT_FALSE(theRegion.regionContainsProvince(1));
}

TEST(EU4World_Regions_RegionTests, oldStyleLoadViaProvincesWorks)
{
	const std::set<int> input{1, 5, 9};
	const EU4::Region theRegion(input);

	ASSERT_TRUE(theRegion.regionContainsProvince(1));
	ASSERT_TRUE(theRegion.regionContainsProvince(5));
	ASSERT_TRUE(theRegion.regionContainsProvince(9));
}

TEST(EU4World_Regions_RegionTests, noAreasMeansNoProvinces)
{
	std::stringstream regionInput;
	regionInput << "areas = {}\n";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);

	EU4::Region theRegion(regionInput);
	theRegion.addProvinces(theAreas);

	ASSERT_FALSE(theRegion.regionContainsProvince(1));
}

TEST(EU4World_Regions_RegionTests, AreasProvideProvinceMatches)
{
	std::stringstream regionInput;
	regionInput << "areas = {\n";
	regionInput << "\ttest_area\n";
	regionInput << "}\n";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);

	EU4::Region theRegion(regionInput);
	theRegion.addProvinces(theAreas);

	ASSERT_TRUE(theRegion.regionContainsProvince(1));
	ASSERT_TRUE(theRegion.regionContainsProvince(2));
	ASSERT_TRUE(theRegion.regionContainsProvince(3));
	ASSERT_TRUE(theRegion.areaContainsProvince("test_area", 1));
	ASSERT_TRUE(theRegion.areaContainsProvince("test_area", 2));
	ASSERT_TRUE(theRegion.areaContainsProvince("test_area", 3));
}

TEST(EU4World_Regions_RegionTests, regionStoresLoadedAreaNames)
{
	std::stringstream regionInput;
	regionInput << "areas = {\n";
	regionInput << "\ttest_area test_area2\n";
	regionInput << "}\n";
	const EU4::Region theRegion(regionInput);

	ASSERT_EQ(2, theRegion.getAreaNames().size());
	ASSERT_TRUE(theRegion.getAreaNames().contains("test_area"));
	ASSERT_TRUE(theRegion.getAreaNames().contains("test_area2"));
}