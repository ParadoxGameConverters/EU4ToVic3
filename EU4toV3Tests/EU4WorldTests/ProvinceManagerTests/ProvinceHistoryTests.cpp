#include "ProvinceManager/ProvinceHistory.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_ProvinceHistoryTests, originalDevelopmentIsMarked)
{
	std::stringstream input;
	input << "base_tax = 3\n";
	input << "base_production = 2\n";
	input << "base_manpower = 1\n";
	const EU4::ProvinceHistory theHistory(input);

	EXPECT_DOUBLE_EQ(6.0, theHistory.getOriginalDevelopment());
}

TEST(EU4World_ProvinceHistoryTests, startingCultureCanBeLoadedAndOverridden)
{
	std::stringstream input;
	input << "culture = dummy\n";
	EU4::ProvinceHistory theHistory(input);

	EXPECT_EQ("dummy", theHistory.getStartingCulture());

	theHistory.setStartingCulture("culture");

	EXPECT_EQ("culture", theHistory.getStartingCulture());
}

TEST(EU4World_ProvinceHistoryTests, startingReligionCanBeLoadedAndOverridden)
{
	std::stringstream input;
	input << "religion = dummy\n";
	EU4::ProvinceHistory theHistory(input);

	EXPECT_EQ("dummy", theHistory.getStartingReligion());

	theHistory.setStartingReligion("religion");

	EXPECT_EQ("religion", theHistory.getStartingReligion());
}

TEST(EU4World_ProvinceHistoryTests, historyIsInitializedForBothCultureAndReligion)
{
	std::stringstream input1;
	EU4::ProvinceHistory theHistory1(input1);
	std::stringstream input2;
	input2 << "culture = dummy_culture\n";
	EU4::ProvinceHistory theHistory2(input2);
	std::stringstream input3;
	input3 << "religion = dummy_religion\n";
	EU4::ProvinceHistory theHistory3(input3);
	std::stringstream input4;
	input4 << "culture = dummy_culture\n";
	input4 << "religion = dummy_religion\n";
	EU4::ProvinceHistory theHistory4(input4);

	EXPECT_FALSE(theHistory1.hasInitializedHistory());
	EXPECT_FALSE(theHistory2.hasInitializedHistory());
	EXPECT_FALSE(theHistory3.hasInitializedHistory());
	EXPECT_TRUE(theHistory4.hasInitializedHistory());
}

TEST(EU4World_ProvinceHistoryTests, dateItemsStoreEntries)
{
	std::stringstream input;
	input << "1.1.1 = { owner = t1 }\n";
	input << "1.1.2 = { culture = c1 }\n";
	input << "1.1.3 = { religion = r1 }\n";
	input << "1.1.4 = { owner = t2 }\n";
	input << "1.1.5 = { culture = c2 }\n";
	input << "1.1.6 = { religion = r2 }\n";
	EU4::ProvinceHistory theHistory(input);

	EXPECT_THAT(theHistory.getOwnershipHistory(),
		 UnorderedElementsAre(std::pair<date, std::string>(date("1.1.1"), "t1"), std::pair<date, std::string>(date("1.1.4"), "t2")));
	EXPECT_THAT(theHistory.getCultureHistory(),
		 UnorderedElementsAre(std::pair<date, std::string>(date("1.1.2"), "c1"), std::pair<date, std::string>(date("1.1.5"), "c2")));
	EXPECT_THAT(theHistory.getReligionHistory(),
		 UnorderedElementsAre(std::pair<date, std::string>(date("1.1.3"), "r1"), std::pair<date, std::string>(date("1.1.6"), "r2")));
}
