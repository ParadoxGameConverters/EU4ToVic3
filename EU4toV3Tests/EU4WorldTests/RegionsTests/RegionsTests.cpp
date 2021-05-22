#include "gtest/gtest.h"
#include "Regions/Regions.h"
#include "Regions/Areas.h"
#include "Regions/SuperRegions.h"

TEST(EU4World_Regions_RegionsTests, oldStyleNoProvincesMeansNoProvinces)
{
	std::stringstream areasInput;
	areasInput << "test_region = {\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(theAreas);

	ASSERT_FALSE(theRegions.provinceInRegion(1, "test_region"));
}

TEST(EU4World_Regions_RegionsTests, oldStyleProvincesCanBeSet)
{
	std::stringstream areasInput;
	areasInput << "test_region = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(theAreas);

	ASSERT_TRUE(theRegions.provinceInRegion(1, "test_region"));
}

TEST(EU4World_Regions_RegionsTests, newStyleNoAreasMeansNoProvinces)
{
	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = {";
	superRegionsInput << "\ttest_region";
	superRegionsInput << "}";
	EU4::SuperRegions superRegions(superRegionsInput);
	
	std::stringstream regionsInput;
	regionsInput << "test_region = {\n";
	regionsInput << "\tareas = {\n";
	regionsInput << "\t}\n";
	regionsInput << "}";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(superRegions, theAreas, regionsInput);

	ASSERT_FALSE(theRegions.provinceInRegion(1, "test_region"));
}

TEST(EU4World_Regions_RegionsTests, newStyleNoProvincesMeansNoProvinces)
{
	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = {";
	superRegionsInput << "\ttest_region";
	superRegionsInput << "}";
	EU4::SuperRegions superRegions(superRegionsInput);

	std::stringstream regionsInput;
	regionsInput << "test_region = {\n";
	regionsInput << "\tareas = {\n";
	regionsInput << "\t\ttest_area\n";
	regionsInput << "\t}\n";
	regionsInput << "}";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(superRegions, theAreas, regionsInput);

	ASSERT_FALSE(theRegions.provinceInRegion(1, "test_region"));
}

TEST(EU4World_Regions_RegionsTests, newStyleAreasProvideProvinces)
{
	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = {";
	superRegionsInput << "\ttest_region";
	superRegionsInput << "}";
	EU4::SuperRegions superRegions(superRegionsInput);

	std::stringstream regionsInput;
	regionsInput << "test_region = {\n";
	regionsInput << "\tareas = {\n";
	regionsInput << "\t\ttest_area\n";
	regionsInput << "\t}\n";
	regionsInput << "}";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(superRegions, theAreas, regionsInput);

	ASSERT_TRUE(theRegions.provinceInRegion(1, "test_region"));
}

TEST(EU4World_Regions_RegionsTests, newStyleAreasProvideProvincesBySuperRegion)
{
	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = {";
	superRegionsInput << "\ttest_region";
	superRegionsInput << "}";
	EU4::SuperRegions superRegions(superRegionsInput);

	std::stringstream regionsInput;
	regionsInput << "test_region = {\n";
	regionsInput << "\tareas = {\n";
	regionsInput << "\t\ttest_area\n";
	regionsInput << "\t}\n";
	regionsInput << "}";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);

	EU4::Regions theRegions(superRegions, theAreas, regionsInput);

	ASSERT_TRUE(theRegions.provinceInRegion(1, "test_superregion"));
}

TEST(EU4World_Regions_RegionsTests, newStyleAreasProvideProvincesByArea)
{
	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = {";
	superRegionsInput << "\ttest_region";
	superRegionsInput << "}";
	EU4::SuperRegions superRegions(superRegionsInput);

	std::stringstream regionsInput;
	regionsInput << "test_region = {\n";
	regionsInput << "\tareas = {\n";
	regionsInput << "\t\ttest_area\n";
	regionsInput << "\t}\n";
	regionsInput << "}";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);

	EU4::Regions theRegions(superRegions, theAreas, regionsInput);

	ASSERT_TRUE(theRegions.provinceInRegion(1, "test_area"));
}

TEST(EU4World_Regions_RegionsTests, oldStyleParentNamesReturnNulloptOnMismatch)
{
	std::stringstream areasInput;
	areasInput << "test_region = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(theAreas);

	ASSERT_EQ(std::nullopt, theRegions.getParentAreaName(9));
	ASSERT_EQ(std::nullopt, theRegions.getParentRegionName(9));
	ASSERT_EQ(std::nullopt, theRegions.getParentSuperRegionName(9));
}

TEST(EU4World_Regions_RegionsTests, newStyleParentNamesReturnNulloptOnMismatch)
{
	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = {";
	superRegionsInput << "\ttest_region";
	superRegionsInput << "}";
	EU4::SuperRegions superRegions(superRegionsInput);

	std::stringstream regionsInput;
	regionsInput << "test_region = {\n";
	regionsInput << "\tareas = {\n";
	regionsInput << "\t\ttest_area\n";
	regionsInput << "\t}\n";
	regionsInput << "}";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(superRegions, theAreas, regionsInput);

	ASSERT_EQ(std::nullopt, theRegions.getParentAreaName(9));
	ASSERT_EQ(std::nullopt, theRegions.getParentRegionName(9));
	ASSERT_EQ(std::nullopt, theRegions.getParentSuperRegionName(9));
}

TEST(EU4World_Regions_RegionsTests, oldStyleParentNamesCanBeRetrievedButItsADummyOrNullopt)
{
	std::stringstream areasInput;
	areasInput << "test_region = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(theAreas);

	ASSERT_EQ("dummyArea", *theRegions.getParentAreaName(1));
	ASSERT_EQ("test_region", *theRegions.getParentRegionName(1));
	ASSERT_EQ(std::nullopt, theRegions.getParentSuperRegionName(1));
}

TEST(EU4World_Regions_RegionsTests, newStyleParentNamesCanBeRetrieved)
{
	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = {";
	superRegionsInput << "\ttest_region";
	superRegionsInput << "}";
	EU4::SuperRegions superRegions(superRegionsInput);

	std::stringstream regionsInput;
	regionsInput << "test_region = {\n";
	regionsInput << "\tareas = {\n";
	regionsInput << "\t\ttest_area\n";
	regionsInput << "\t}\n";
	regionsInput << "}";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(superRegions, theAreas, regionsInput);

	ASSERT_EQ("test_area", *theRegions.getParentAreaName(1));
	ASSERT_EQ("test_region", *theRegions.getParentRegionName(1));
	ASSERT_EQ("test_superregion", *theRegions.getParentSuperRegionName(1));
}

TEST(EU4World_Regions_RegionsTests, oldStyleRegionNameValidityCanBeDetermined)
{
	std::stringstream areasInput;
	areasInput << "test_region = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(theAreas);

	ASSERT_TRUE(theRegions.regionIsValid("test_region"));
	ASSERT_FALSE(theRegions.regionIsValid("test_region2"));
}

TEST(EU4World_Regions_RegionsTests, newStyleRegionNameValidityCanBeDetermined)
{
	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = {";
	superRegionsInput << "\ttest_region";
	superRegionsInput << "}";
	EU4::SuperRegions superRegions(superRegionsInput);

	std::stringstream regionsInput;
	regionsInput << "test_region = {\n";
	regionsInput << "\tareas = {\n";
	regionsInput << "\t\ttest_area\n";
	regionsInput << "\t}\n";
	regionsInput << "}";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(superRegions, theAreas, regionsInput);

	ASSERT_TRUE(theRegions.regionIsValid("test_superregion"));
	ASSERT_TRUE(theRegions.regionIsValid("test_region"));
	ASSERT_TRUE(theRegions.regionIsValid("test_area"));
	ASSERT_FALSE(theRegions.regionIsValid("test_mismatch"));
}
