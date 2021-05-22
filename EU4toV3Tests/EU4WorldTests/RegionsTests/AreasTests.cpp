#include "Regions/Areas.h"
#include "gtest/gtest.h"

TEST(EU4World_Regions_AreasTests, nonMatchingAreaGivesNoProvinces)
{
	std::stringstream input;
	input << "an_area = {}";
	const EU4::Areas theAreas(input);

	ASSERT_EQ(std::nullopt, theAreas.getProvincesInArea("non_matching_area"));
}

TEST(EU4World_Regions_AreasTests, matchingAreaGivesProvinces)
{
	std::stringstream input;
	input << "matching_area = { 56 57 }";
	const EU4::Areas theAreas(input);

	const auto& provinces = theAreas.getProvincesInArea("matching_area");
	const auto result = std::set<int>{56, 57};

	ASSERT_EQ(result, *provinces);
}

TEST(EU4World_Regions_AreasTests, matchingWorksforMultipleAreas)
{
	std::stringstream input;
	input << "non_matching_area = { 54 55 }";
	input << "matching_area = { 56 57 }";
	input << "non_matching_area2 = { 58 59 }";
	const EU4::Areas theAreas(input);

	const auto& provinces = theAreas.getProvincesInArea("matching_area");
	const auto result = std::set<int>{56, 57};

	ASSERT_EQ(result, *provinces);
}

TEST(EU4World_Regions_AreasTests, areasAreReturned)
{
	std::stringstream input;
	input << "area1 = { 54 55 }";
	input << "area2 = { 56 57 }";
	input << "area3 = { 58 59 60 }";
	const EU4::Areas theAreas(input);

	ASSERT_EQ("area1", theAreas.getAreas().find("area1")->first);
	ASSERT_EQ(2, theAreas.getAreas().find("area1")->second.size());
	ASSERT_EQ("area2", theAreas.getAreas().find("area2")->first);
	ASSERT_EQ(2, theAreas.getAreas().find("area2")->second.size());
	ASSERT_EQ("area3", theAreas.getAreas().find("area3")->first);
	ASSERT_EQ(3, theAreas.getAreas().find("area3")->second.size());
}
