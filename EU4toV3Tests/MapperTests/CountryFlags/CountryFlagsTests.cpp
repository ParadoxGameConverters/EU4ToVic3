#include "CountryFlags/CountryFlags.h"
#include "gtest/gtest.h"

TEST(Mappers_CountryFlagsTests, VerifyCountryFlags)
{
	std::stringstream input;
	input << "link = { eu4 = changed_from_colonial_nation v2 = post_colonial_country v2 = postcol_1 }";
	input << "link = { eu4 = eu4Flag v2 = v2Flag1 v2 = v2Flag2 }";
	input << "link = { eu4 = changed_from_colonial_nation v2 = post_colonial_country v2 = postcol_2 }";
	mappers::CountryFlags countryFlags(input);

	auto flagRangeItr = countryFlags.getFlags().equal_range("changed_from_colonial_nation");
	std::vector<std::string> v2Flags;
	while (flagRangeItr.first != flagRangeItr.second)
	{
		v2Flags.push_back(flagRangeItr.first->second);
		++flagRangeItr.first;
	}

	ASSERT_TRUE(std::find(v2Flags.begin(), v2Flags.end(), "post_colonial_country") != v2Flags.end());
	ASSERT_TRUE(std::find(v2Flags.begin(), v2Flags.end(), "postcol_1") != v2Flags.end());
	ASSERT_TRUE(std::find(v2Flags.begin(), v2Flags.end(), "postcol_2") != v2Flags.end());
	ASSERT_FALSE(std::find(v2Flags.begin(), v2Flags.end(), "v2Flag1") != v2Flags.end());
	ASSERT_FALSE(std::find(v2Flags.begin(), v2Flags.end(), "v2Flag2") != v2Flags.end());
}
