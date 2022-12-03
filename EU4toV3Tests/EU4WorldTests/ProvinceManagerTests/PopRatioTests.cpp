#include "ProvinceManager/PopRatio.h"
#include "gtest/gtest.h"

TEST(EU4World_PopRatioTests, defaultConstructionCorrect)
{
	const EU4::PopRatio theRatio("theCulture", "theReligion");

	EXPECT_EQ("theCulture", theRatio.getCulture());
	EXPECT_EQ("theReligion", theRatio.getReligion());
	EXPECT_DOUBLE_EQ(1.0, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(1.0, theRatio.getMiddleRatio());
	EXPECT_DOUBLE_EQ(1.0, theRatio.getLowerRatio());
}

TEST(EU4World_PopRatioTests, decayByAssimilationValuePerYear)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.decay(1, 0.0025); // default assimilation value "1" is 0.0025 (factor = (1.0 - pow(0.75, value)) / 100;)

	EXPECT_DOUBLE_EQ(0.9975, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(0.9975, theRatio.getMiddleRatio());
	EXPECT_DOUBLE_EQ(0.9975, theRatio.getLowerRatio());
}

TEST(EU4World_PopRatioTests, decayForHigherAssimilationValue)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.decay(1, 0.068359375); // assimilation value "4" is 0.06835 (factor = (1.0 - pow(0.75, 4)) / 100;)

	EXPECT_DOUBLE_EQ(0.931640625, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(0.931640625, theRatio.getMiddleRatio());
	EXPECT_DOUBLE_EQ(0.931640625, theRatio.getLowerRatio());
}

TEST(EU4World_PopRatioTests, decayByZeroAssimilationDoesNoAffectRatios)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.decay(1, 0); // No assimilation

	EXPECT_DOUBLE_EQ(1, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(1, theRatio.getMiddleRatio());
	EXPECT_DOUBLE_EQ(1, theRatio.getLowerRatio());
}

TEST(EU4World_PopRatioTests, increaseByDefaultAssimilationValue)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion"); // ratios are all at 1, we cannot increase them.
	theRatio.convertTo("newCulture", "newReligion");	  // halving and zeroing ratios to drive them lower.
	theRatio.increase(1, 0.0025);								  // default assimilation value "1" is 0.0025 (factor = (1.0 - pow(0.75, value)) / 100;)

	EXPECT_DOUBLE_EQ(0.50125, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(0.50125, theRatio.getMiddleRatio());
	EXPECT_NEAR(0.0025, theRatio.getLowerRatio(), 0.001);
}

TEST(EU4World_PopRatioTests, increaseByHigherAssimilationValue)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion"); // ratios are all at 1, we cannot increase them.
	theRatio.convertTo("newCulture", "newReligion");	  // halving and zeroing ratios to drive them lower.
	theRatio.increase(1, 0.068359375);						  // assimilation value "4" is 0.06835 (factor = (1.0 - pow(0.75, 4)) / 100;)

	EXPECT_DOUBLE_EQ(0.5341796875, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(0.5341796875, theRatio.getMiddleRatio());
	EXPECT_DOUBLE_EQ(0.068359375, theRatio.getLowerRatio());
}

TEST(EU4World_PopRatioTests, increaseByZeroDoesNotAffectPopRatios)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion"); // ratios are all at 1, we cannot increase them.
	theRatio.convertTo("newCulture", "newReligion");	  // halving and zeroing ratios to drive them lower.
	theRatio.increase(1, 0);									  // No assimilation

	EXPECT_DOUBLE_EQ(0.5, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(0.5, theRatio.getMiddleRatio());
	EXPECT_DOUBLE_EQ(0, theRatio.getLowerRatio());
}

TEST(EU4World_PopRatioTests, convertingFromDecreasesUpperAndMiddleRatio)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertFrom();

	EXPECT_DOUBLE_EQ(0.5, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(0.5, theRatio.getMiddleRatio());
}

TEST(EU4World_PopRatioTests, convertingFromDoesNotDecreaseLowerRatio)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertFrom();

	EXPECT_DOUBLE_EQ(1.0, theRatio.getLowerRatio());
}

TEST(EU4World_PopRatioTests, convertingToCultureModifiesCulture)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertToCulture("newCulture");

	EXPECT_EQ("newCulture", theRatio.getCulture());
}

TEST(EU4World_PopRatioTests, convertingToCultureHalvesUpperAndMiddleRatio)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertToCulture("newCulture");

	EXPECT_DOUBLE_EQ(0.5, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(0.5, theRatio.getMiddleRatio());
}

TEST(EU4World_PopRatioTests, convertingToCultureZeroesLowerRatio)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertToCulture("newCulture");

	EXPECT_EQ(0.0, theRatio.getLowerRatio());
}

TEST(EU4World_PopRatioTests, convertingToReligionModifiesReligion)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertToReligion("newReligion");

	EXPECT_EQ("newReligion", theRatio.getReligion());
}

TEST(EU4World_PopRatioTests, convertingToReligionHalvesUpperAndMiddleRatio)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertToReligion("newReligion");

	EXPECT_DOUBLE_EQ(0.5, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(0.5, theRatio.getMiddleRatio());
}

TEST(EU4World_PopRatioTests, convertingToReligionZeroesLowerRatio)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertToReligion("newReligion");

	EXPECT_EQ(0.0, theRatio.getLowerRatio());
}

TEST(EU4World_PopRatioTests, convertingToModifiesReligionAndCulture)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertTo("newCulture", "newReligion");

	EXPECT_EQ("newCulture", theRatio.getCulture());
	EXPECT_EQ("newReligion", theRatio.getReligion());
}

TEST(EU4World_PopRatioTests, convertingToHavesUpperAndMiddleRatio)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertTo("newCulture", "newReligion");

	EXPECT_DOUBLE_EQ(0.5, theRatio.getUpperRatio());
	EXPECT_DOUBLE_EQ(0.5, theRatio.getMiddleRatio());
}

TEST(EU4World_PopRatioTests, convertingToZeroesLowerRatio)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.convertTo("newCulture", "newReligion");

	EXPECT_EQ(0.0, theRatio.getLowerRatio());
}

TEST(EU4World_PopRatioTests, multiplyByFactor)
{
	std::set<int> input;

	EU4::PopRatio theRatio("theCulture", "theReligion");
	theRatio.multiplyRatios(0.5);

	EXPECT_EQ(0.5, theRatio.getUpperRatio());
	EXPECT_EQ(0.5, theRatio.getMiddleRatio());
	EXPECT_EQ(0.5, theRatio.getLowerRatio());

	theRatio.multiplyRatios(0.5);

	EXPECT_EQ(0.25, theRatio.getUpperRatio());
	EXPECT_EQ(0.25, theRatio.getMiddleRatio());
	EXPECT_EQ(0.25, theRatio.getLowerRatio());
}