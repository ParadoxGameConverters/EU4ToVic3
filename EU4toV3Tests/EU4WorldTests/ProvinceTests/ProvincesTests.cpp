#include "gtest/gtest.h"
#include "Provinces/Provinces.h"
#include "ProvinceMappings/ProvinceMapper.h"

TEST(EU4World_ProvincesTests, defaultProvincesIsEmpty)
{
	std::stringstream input;
	const EU4::Provinces theProvinces(input);
	
	ASSERT_TRUE(theProvinces.getAllProvinces().empty());
}

TEST(EU4World_ProvincesTests, provincesCanBeInput)
{
	std::stringstream input;
	input << "-1={}";
	const EU4::Provinces theProvinces(input);
	
	ASSERT_EQ(1, theProvinces.getAllProvinces().size());
}

TEST(EU4World_ProvincesTests, gettingNonExistentProvinceThrowsException)
{
	std::stringstream input;
	input << "-1={}";
	const EU4::Provinces theProvinces(input);
	
	ASSERT_THROW(const auto& breakPlease = theProvinces.getProvince(42), std::range_error);
}

TEST(EU4World_ProvincesTests, provinceCanBeRetrieved)
{
	std::stringstream input;
	input << "-1={}";
	const EU4::Provinces theProvinces(input);
	
	ASSERT_EQ(1, theProvinces.getProvince(1)->getNum());
}
