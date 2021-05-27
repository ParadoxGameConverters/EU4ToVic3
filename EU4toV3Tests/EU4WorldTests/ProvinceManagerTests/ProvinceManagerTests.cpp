#include "ProvinceManager/ProvinceManager.h"
#include "gtest/gtest.h"

TEST(EU4World_ProvinceManagerTests, defaultProvinceManagerIsEmpty)
{
	std::stringstream input;
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(input);

	EXPECT_TRUE(theProvinceManager.getAllProvinces().empty());
}

TEST(EU4World_ProvinceManagerTests, ProvinceManagerCanBeInput)
{
	std::stringstream input;
	input << "-1={}";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(input);

	EXPECT_TRUE(theProvinceManager.getAllProvinces().contains(1));
}

TEST(EU4World_ProvinceManagerTests, gettingNonExistentProvinceThrowsException)
{
	std::stringstream input;
	input << "-1={}";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(input);

	EXPECT_THROW(const auto& breakPlease = theProvinceManager.getProvince(42), std::range_error);
}

TEST(EU4World_ProvinceManagerTests, provinceCanBeRetrieved)
{
	std::stringstream input;
	input << "-1={}";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(input);

	EXPECT_EQ(1, theProvinceManager.getProvince(1)->getID());
}
