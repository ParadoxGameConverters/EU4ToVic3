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

TEST(EU4World_ProvinceHistoryTests, popRatiosForTrivialHistoryAreOne)
{
	DatingData datingData;
	datingData.startEU4Date = date("1254.11.11");

	std::stringstream input;
	input << "culture=origCulture\n";
	input << "religion=origReligion\n";
	EU4::ProvinceHistory theHistory(input);
	theHistory.buildPopRatios(0.0025, datingData); // using default assimilation factor.

	const auto& popRatios = theHistory.getPopRatios();

	ASSERT_EQ(1, popRatios.size());

	// There is a single pop ratio filling entire pie chart
	const auto& pop1 = popRatios[0];
	EXPECT_EQ("origCulture", pop1.getCulture());
	EXPECT_EQ("origReligion", pop1.getReligion());
	EXPECT_NEAR(1, pop1.getUpperRatio(), 0.0000001);
	EXPECT_NEAR(1, pop1.getMiddleRatio(), 0.0000001);
	EXPECT_NEAR(1, pop1.getLowerRatio(), 0.0000001);
}

TEST(EU4World_ProvinceHistoryTests, popRatiosForColonizationAreCalculated)
{
	DatingData datingData;
	datingData.startEU4Date = date("1254.11.11");
	datingData.lastEU4Date = date("1737.1.1");

	std::stringstream input;
	input << "culture=origCulture\n";
	input << "religion=origReligion\n";
	input << "1736.1.1={\n"; // colonized 1 year before end date
	input << "\towner=TAG1\n";
	input << "\tculture=TAG1Culture\n";	  // converted culture
	input << "\treligion=TAG1Religion\n"; // converted religion
	input << "}\n";
	EU4::ProvinceHistory theHistory(input);
	theHistory.buildPopRatios(0.0025, datingData); // using default assimilation factor.

	const auto& popRatios = theHistory.getPopRatios();

	EXPECT_EQ(2, popRatios.size());

	// First ratio is the original one.
	const auto& pop1 = popRatios[0];
	EXPECT_EQ("origCulture", pop1.getCulture());
	EXPECT_EQ("origReligion", pop1.getReligion());
	EXPECT_NEAR(0.49875, pop1.getUpperRatio(), 0.0000001);
	EXPECT_NEAR(0.49875, pop1.getMiddleRatio(), 0.0000001);
	EXPECT_NEAR(0.99750, pop1.getLowerRatio(), 0.0000001); // Majority of peasants are still here, they assimilate very hard.

	const auto& pop2 = popRatios[1];
	EXPECT_EQ("TAG1Culture", pop2.getCulture());
	EXPECT_EQ("TAG1Religion", pop2.getReligion());
	EXPECT_NEAR(0.50125, pop2.getUpperRatio(), 0.0000001); // we converted half, and then decayed by 1 year, which is remaining 50% * 0.0025
	EXPECT_NEAR(0.50125, pop2.getMiddleRatio(), 0.0000001);
	EXPECT_NEAR(0.00250, pop2.getLowerRatio(), 0.0000001); // 0.0025 * remaining 100%
}

TEST(EU4World_ProvinceHistoryTests, provinceHistoryCanBePurgedForSimplePopRatio)
{
	DatingData datingData;
	datingData.startEU4Date = date("1254.11.11");
	datingData.lastEU4Date = date("1737.1.1");

	std::stringstream input;
	input << "culture=origCulture\n";
	input << "religion=origReligion\n";
	input << "1736.1.1={\n"; // colonized 1 year before end date
	input << "\towner=TAG1\n";
	input << "\tculture=TAG1Culture\n";	  // converted culture
	input << "\treligion=TAG1Religion\n"; // converted religion
	input << "}\n";
	EU4::ProvinceHistory theHistory(input);

	theHistory.purgeHistories();

	ASSERT_EQ(1, theHistory.getCultureHistory().size());
	const auto& cultureEntry = theHistory.getCultureHistory()[0];
	EXPECT_EQ(date("1.1.1"), cultureEntry.first);
	EXPECT_EQ("TAG1Culture", cultureEntry.second);

	ASSERT_EQ(1, theHistory.getReligionHistory().size());
	const auto& religionEntry = theHistory.getReligionHistory()[0];
	EXPECT_EQ(date("1.1.1"), religionEntry.first);
	EXPECT_EQ("TAG1Religion", religionEntry.second);

	theHistory.buildPopRatios(0.0025, datingData); // using default assimilation factor.

	const auto& popRatios = theHistory.getPopRatios();

	ASSERT_EQ(1, popRatios.size());

	const auto& pop = popRatios[0];
	EXPECT_EQ("TAG1Culture", pop.getCulture());
	EXPECT_EQ("TAG1Religion", pop.getReligion());
	EXPECT_NEAR(1, pop.getUpperRatio(), 0.0000001);
	EXPECT_NEAR(1, pop.getMiddleRatio(), 0.0000001);
	EXPECT_NEAR(1, pop.getLowerRatio(), 0.0000001);
}

TEST(EU4World_ProvinceHistoryTests, popRatiosCanBeCombined)
{
	DatingData datingData;
	datingData.startEU4Date = date("1254.11.11");
	datingData.lastEU4Date = date("1737.1.1");

	std::stringstream input;
	input << "culture=origCulture\n";
	input << "religion=origReligion\n";
	input << "1736.1.1={\n"; // colonized 1 year before end date
	input << "\towner=TAG1\n";
	input << "\tculture=TAG1Culture\n";	  // converted culture
	input << "\treligion=TAG1Religion\n"; // converted religion
	input << "}\n";
	input << "1736.7.1={\n"; // recolonized 1/2 year before end date
	input << "\towner=TAG1\n";
	input << "\tculture=TAG1Culture\n";	  // converted culture
	input << "\treligion=TAG1Religion\n"; // converted religion
	input << "}\n";
	EU4::ProvinceHistory theHistory(input);
	theHistory.buildPopRatios(0.0025, datingData); // using default assimilation factor.

	const auto& popRatios = theHistory.getPopRatios();

	ASSERT_EQ(2, popRatios.size());

	// First ratio is the original one.
	const auto& pop1 = popRatios[0];
	EXPECT_EQ("origCulture", pop1.getCulture());
	EXPECT_EQ("origReligion", pop1.getReligion());
	EXPECT_NEAR(0.49875, pop1.getUpperRatio(), 0.0000001);
	EXPECT_NEAR(0.49875, pop1.getMiddleRatio(), 0.0000001);
	EXPECT_NEAR(0.99750, pop1.getLowerRatio(), 0.0000001); // Majority of peasants are still here, they assimilate very hard.

	const auto& pop2 = popRatios[1];
	EXPECT_EQ("TAG1Culture", pop2.getCulture());
	EXPECT_EQ("TAG1Religion", pop2.getReligion());
	EXPECT_NEAR(0.50125, pop2.getUpperRatio(), 0.0000001); // we converted half, and then decayed by 1 year, which is remaining 50% * 0.0025
	EXPECT_NEAR(0.50125, pop2.getMiddleRatio(), 0.0000001);
	EXPECT_NEAR(0.00250, pop2.getLowerRatio(), 0.0000001); // 0.0025 * remaining 100%
}

TEST(EU4World_ProvinceHistoryTests, popRatiosCanBeConstructed)
{
	DatingData datingData;
	datingData.startEU4Date = date("1254.11.11");

	std::stringstream input;
	input << "culture=origCulture\n";
	input << "religion=origReligion\n";
	input << "1600.1.1={\n"; // colonized
	input << "\towner=TAG1\n";
	input << "\tculture=TAG1Culture\n"; // converted culture
	input << "}\n";
	input << "1600.1.10={\n";				// colonized (EU4 error, this entry will be discarded)
	input << "\tculture=TAG1Culture\n"; // converted culture
	input << "}\n";
	input << "1600.1.20={\n";				// colonized (EU4 error, this entry will be discarded)
	input << "\tculture=TAG1Culture\n"; // converted culture
	input << "}\n";
	input << "1602.1.1={\n"; // converted religion
	input << "\treligion=TAG1Religion\n";
	input << "}\n";
	input << "1603.1.1={\n"; // conquered
	input << "\towner=TAG2\n";
	input << "}\n";
	input << "1605.1.1={\n"; // converted religion and culture on same day.
	input << "\tculture=TAG2Culture\n";
	input << "\treligion=TAG2Religion\n";
	input << "}\n";
	EU4::ProvinceHistory theHistory(input);
	theHistory.buildPopRatios(0.0025, datingData); // using default assimilation factor.

	const auto& popRatios = theHistory.getPopRatios();

	ASSERT_EQ(4, popRatios.size());

	// First ratio is the original one. It has been decayed by 236 years (1836 - 1600).
	const auto& pop1 = popRatios[0];
	EXPECT_EQ("origCulture", pop1.getCulture());
	EXPECT_EQ("origReligion", pop1.getReligion());
	EXPECT_NEAR(0.0692397, pop1.getUpperRatio(), 0.0000001);
	EXPECT_NEAR(0.0692397, pop1.getMiddleRatio(), 0.0000001);
	EXPECT_NEAR(0.5539179, pop1.getLowerRatio(), 0.0000001); // Majority of peasants are still here, they assimilate very hard.

	// Second ratio is the tag1Culture ratio from 1602, decayed by 234 years (1836 - 1602).
	const auto& pop2 = popRatios[1];
	EXPECT_EQ("TAG1Culture", pop2.getCulture());
	EXPECT_EQ("origReligion", pop2.getReligion());
	EXPECT_NEAR(0.0699347, pop2.getUpperRatio(), 0.0000001);
	EXPECT_NEAR(0.0699347, pop2.getMiddleRatio(), 0.0000001);
	EXPECT_NEAR(0.0027800, pop2.getLowerRatio(), 0.0000001); // There aren't all that many peasants that converted in those 2 years.

	// Third ratio is from 1605, for Tag1culture/Tag1Religion, for the few folks that also converted religion, decayed by 231 years  (1836 - 1605).
	const auto& pop3 = popRatios[2];
	EXPECT_EQ("TAG1Culture", pop3.getCulture());
	EXPECT_EQ("TAG1Religion", pop3.getReligion());
	EXPECT_NEAR(0.1412725, pop3.getUpperRatio(), 0.0000001);
	EXPECT_NEAR(0.1412725, pop3.getMiddleRatio(), 0.0000001);
	EXPECT_NEAR(0.0041961, pop3.getLowerRatio(), 0.0000001); // Very few people assimilated in 3 years.

	// Final ratio is the current (game end date) ratio (defaults to 1836), for Tag2culture/Tag2Religion, decayed by 0 years (1836 - 1836).
	const auto& pop4 = popRatios[3];
	EXPECT_EQ("TAG2Culture", pop4.getCulture());
	EXPECT_EQ("TAG2Religion", pop4.getReligion());
	EXPECT_NEAR(0.7195529, pop4.getUpperRatio(), 0.0000001); // Most of nobility and middle class have assimilated.
	EXPECT_NEAR(0.7195529, pop4.getMiddleRatio(), 0.0000001);
	EXPECT_NEAR(0.4391058, pop4.getLowerRatio(), 0.0000001); // This is where most of assimilated peasants are.

	// Confirm sums are mostly correct:
	EXPECT_NEAR(1, pop1.getUpperRatio() + pop2.getUpperRatio() + pop3.getUpperRatio() + pop4.getUpperRatio(), 0.001);
	EXPECT_NEAR(1, pop1.getMiddleRatio() + pop2.getMiddleRatio() + pop3.getMiddleRatio() + pop4.getMiddleRatio(), 0.001);
	EXPECT_NEAR(1, pop1.getLowerRatio() + pop2.getLowerRatio() + pop3.getLowerRatio() + pop4.getLowerRatio(), 0.001);
}
