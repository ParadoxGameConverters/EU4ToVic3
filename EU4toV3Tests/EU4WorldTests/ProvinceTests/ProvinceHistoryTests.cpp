#include "Provinces/ProvinceHistory.h"
#include "gtest/gtest.h"

TEST(EU4World_Province_HistoryTests, firstOwnedDateCanBeEmpty)
{
	std::stringstream input;
	const EU4::ProvinceHistory theHistory(input);

	ASSERT_FALSE(theHistory.getFirstOwnedDate());
}

TEST(EU4World_Province_HistoryTests, firstOwnedDateCanBeSetFromStartingOwner)
{
	std::stringstream input;
	input << "owner=\"TAG\"\n";
	const EU4::ProvinceHistory theHistory(input);
	const auto firstOwnedDate = theHistory.getFirstOwnedDate();

	ASSERT_TRUE(firstOwnedDate);
	ASSERT_EQ(date("1000.1.1"), firstOwnedDate);
}

TEST(EU4World_Province_HistoryTests, firstOwnedDateCanBeSetFromOwnerChange)
{
	std::stringstream input;
	input << "1600.1.1={\n";
	input << "\towner=\"TAG\"\n";
	input << "}\n";
	const EU4::ProvinceHistory theHistory(input);
	const auto firstOwnedDate = theHistory.getFirstOwnedDate();

	ASSERT_TRUE(firstOwnedDate);
	ASSERT_EQ(date("1600.1.1"), firstOwnedDate);
}

TEST(EU4World_Province_HistoryTests, hasOriginalCultureIfNoCulture)
{
	std::stringstream input;
	const EU4::ProvinceHistory theHistory(input);

	ASSERT_TRUE(theHistory.hasOriginalCulture());
}

TEST(EU4World_Province_HistoryTests, hasOriginalCultureIfNoCultureChange)
{
	std::stringstream input;
	input << "culture=theCulture\n";
	const EU4::ProvinceHistory theHistory(input);

	ASSERT_TRUE(theHistory.hasOriginalCulture());
}

TEST(EU4World_Province_HistoryTests, hasNonOriginalCultureIfCultureChange)
{
	std::stringstream input;
	input << "culture=theCulture\n";
	input << "1600.1.1={\n";
	input << "\tculture=newCulture\n";
	input << "}\n";
	const EU4::ProvinceHistory theHistory(input);

	ASSERT_FALSE(theHistory.hasOriginalCulture());
}

TEST(EU4World_Province_HistoryTests, hasOriginalCultureIfCultureChangesBack)
{
	std::stringstream input;
	input << "culture=theCulture\n";
	input << "1600.1.1={\n";
	input << "\tculture=newCulture\n";
	input << "}\n";
	input << "1700.1.1={\n";
	input << "\tculture=theCulture\n";
	input << "}\n";
	const EU4::ProvinceHistory theHistory(input);

	ASSERT_TRUE(theHistory.hasOriginalCulture());
}

TEST(EU4World_Province_HistoryTests, wasNotColonizedIfNoOwner)
{
	std::stringstream input;
	const EU4::ProvinceHistory theHistory(input);

	ASSERT_FALSE(theHistory.wasColonized());
}

TEST(EU4World_Province_HistoryTests, wasNotColonizedIfHasOwnerAtStart)
{
	std::stringstream input;
	input << "owner=TAG\n";
	const EU4::ProvinceHistory theHistory(input);

	ASSERT_FALSE(theHistory.wasColonized());
}

TEST(EU4World_Province_HistoryTests, wasColonizedIfFirstOwnerIsNotAtStart)
{
	std::stringstream input;
	input << "culture=theCulture\n";
	input << "1600.1.1={\n";
	input << "\towner=TAG\n";
	input << "\tculture=newCulture\n";
	input << "}\n";
	const EU4::ProvinceHistory theHistory(input);

	ASSERT_TRUE(theHistory.wasColonized());
}

TEST(EU4World_Province_HistoryTests, wasNotColonizedIfCultureDidntChange)
{
	std::stringstream input;
	input << "culture=theCulture\n";
	input << "1600.1.1={\n";
	input << "\towner=TAG\n";
	input << "\tculture=theCulture\n";
	input << "}\n";
	const EU4::ProvinceHistory theHistory(input);

	ASSERT_FALSE(theHistory.wasColonized());
}

TEST(EU4World_Province_HistoryTests, startingDevelopmentIsMarked)
{
	std::stringstream input;
	input << "base_tax = 3\n";
	input << "base_production = 2\n";
	input << "base_manpower = 1\n";
	const EU4::ProvinceHistory theHistory(input);

	ASSERT_NEAR(6.0, theHistory.getOriginalDevelopment(), 0.001);
}

TEST(EU4World_Province_HistoryTests, popRatiosCanBeConstructed)
{
	std::stringstream input;
	input << "culture=origCulture\n";
	input << "religion=origReligion\n";
	input << "1600.1.1={\n"; // colonized
	input << "\towner=TAG1\n";
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
	theHistory.buildPopRatios(0.0025); // using default assimilation factor.

	const auto& popRatios = theHistory.getPopRatios();

	ASSERT_EQ(4, popRatios.size());

	// First ratio is the original one. It has been decayed by 236 years (1836 - 1600).
	const auto& pop1 = popRatios[0];
	ASSERT_EQ("origCulture", pop1.getCulture());
	ASSERT_EQ("origReligion", pop1.getReligion());
	ASSERT_NEAR(0.0692397, pop1.getUpperRatio(), 0.0000001);
	ASSERT_NEAR(0.0692397, pop1.getMiddleRatio(), 0.0000001);
	ASSERT_NEAR(0.5539179, pop1.getLowerRatio(), 0.0000001); // Majority of peasants are still here, they assimilate very hard.

	// Second ratio is the tag1Culture ratio from 1602, decayed by 234 years (1836 - 1602).
	const auto& pop2 = popRatios[1];
	ASSERT_EQ("TAG1Culture", pop2.getCulture());
	ASSERT_EQ("origReligion", pop2.getReligion());
	ASSERT_NEAR(0.0699347, pop2.getUpperRatio(), 0.0000001);
	ASSERT_NEAR(0.0699347, pop2.getMiddleRatio(), 0.0000001);
	ASSERT_NEAR(0.0027800, pop2.getLowerRatio(), 0.0000001); // There aren't all that many peasants that converted in those 2 years.

	// Third ratio is from 1605, for Tag1culture/Tag1Religion, for the few folks that also converted religion, decayed by 231 years  (1836 - 1605).
	const auto& pop3 = popRatios[2];
	ASSERT_EQ("TAG1Culture", pop3.getCulture());
	ASSERT_EQ("TAG1Religion", pop3.getReligion());
	ASSERT_NEAR(0.1412725, pop3.getUpperRatio(), 0.0000001);
	ASSERT_NEAR(0.1412725, pop3.getMiddleRatio(), 0.0000001);
	ASSERT_NEAR(0.0041961, pop3.getLowerRatio(), 0.0000001); // Very few people assimilated in 3 years.

	// Final ratio is the current (game end date) ratio (defaults to 1836), for Tag2culture/Tag2Religion, decayed by 0 years (1836 - 1836).
	const auto& pop4 = popRatios[3];
	ASSERT_EQ("TAG2Culture", pop4.getCulture());
	ASSERT_EQ("TAG2Religion", pop4.getReligion());
	ASSERT_NEAR(0.7195529, pop4.getUpperRatio(), 0.0000001); // Most of nobility and middle class have assimilated.
	ASSERT_NEAR(0.7195529, pop4.getMiddleRatio(), 0.0000001);
	ASSERT_NEAR(0.4391058, pop4.getLowerRatio(), 0.0000001); // This is where most of assimilated peasants are.

	// Confirm sums are mostly correct:
	ASSERT_NEAR(1, pop1.getUpperRatio() + pop2.getUpperRatio() + pop3.getUpperRatio() + pop4.getUpperRatio(), 0.001);
	ASSERT_NEAR(1, pop1.getMiddleRatio() + pop2.getMiddleRatio() + pop3.getMiddleRatio() + pop4.getMiddleRatio(), 0.001);
	ASSERT_NEAR(1, pop1.getLowerRatio() + pop2.getLowerRatio() + pop3.getLowerRatio() + pop4.getLowerRatio(), 0.001);
}

TEST(EU4World_Province_HistoryTests, startingCultureCanBeOverridden)
{
	std::stringstream input;
	input << "culture = dummy\n";
	EU4::ProvinceHistory theHistory(input);

	ASSERT_EQ("dummy", theHistory.getStartingCulture());

	theHistory.setStartingCulture("culture");

	ASSERT_EQ("culture", theHistory.getStartingCulture());
}

TEST(EU4World_Province_HistoryTests, startingReligionCanBeOverridden)
{
	std::stringstream input;
	input << "religion = dummy\n";
	EU4::ProvinceHistory theHistory(input);

	ASSERT_EQ("dummy", theHistory.getStartingReligion());

	theHistory.setStartingReligion("religion");

	ASSERT_EQ("religion", theHistory.getStartingReligion());
}

TEST(EU4World_Province_HistoryTests, historyIsInitializedForBothCultureAndReligion)
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

	ASSERT_FALSE(theHistory1.hasInitializedHistory());
	ASSERT_FALSE(theHistory2.hasInitializedHistory());
	ASSERT_FALSE(theHistory3.hasInitializedHistory());
	ASSERT_TRUE(theHistory4.hasInitializedHistory());
}

TEST(EU4World_Province_HistoryTests, popratioculturesCanBeOverriddenbyNeocultures)
{
	std::stringstream input;
	input << "culture = aztec\n"; // (popratio1)
	input << "religion = nahuatl\n";
	input << "1500.1.1 = {\n";
	input << "\tculture=spanish\n"; // colonized (popratio2)
	input << "}\n";
	input << "1505.1.1 = {\n";
	input << "\treligion=catholic\n"; // converted religion (popratio3)
	input << "}\n";
	EU4::ProvinceHistory theHistory(input);

	theHistory.buildPopRatios(0.0025);
	theHistory.updatePopRatioCulture("spanish", "mexican", "central_america");

	const auto& popRatio1 = theHistory.getPopRatios()[0];
	const auto& popRatio2 = theHistory.getPopRatios()[1];
	const auto& popRatio3 = theHistory.getPopRatios()[2];

	ASSERT_EQ("aztec", popRatio1.getCulture());
	ASSERT_EQ("nahuatl", popRatio1.getReligion());
	ASSERT_TRUE(popRatio1.getOriginalCulture().empty());
	ASSERT_TRUE(popRatio1.getSuperRegion().empty());

	ASSERT_EQ("mexican", popRatio2.getCulture());
	ASSERT_EQ("spanish", popRatio2.getOriginalCulture());
	ASSERT_EQ("nahuatl", popRatio2.getReligion());
	ASSERT_EQ("central_america", popRatio2.getSuperRegion());

	ASSERT_EQ("mexican", popRatio3.getCulture());
	ASSERT_EQ("spanish", popRatio3.getOriginalCulture());
	ASSERT_EQ("catholic", popRatio3.getReligion());
	ASSERT_EQ("central_america", popRatio3.getSuperRegion());
}
