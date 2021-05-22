#include "gtest/gtest.h"
#include "../EU4toV3/Source/Mappers/TechSchools/TechSchool.h"


TEST(Mappers_TechSchoolTests, MatchingInvestmentCausesScoreOfZero)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_EQ(0, newTechSchool.calculateComparisonScore(0.2,0.2,0.2,0.2,0.2));
}

TEST(Mappers_TechSchoolTests, IncreasingArmyInvestmentIncreasesScore)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_LT(0, newTechSchool.calculateComparisonScore(0.3,0.2,0.2,0.2,0.2));
}

TEST(Mappers_TechSchoolTests, DecreasingArmyInvestmentIncreasesScore)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_LT(0, newTechSchool.calculateComparisonScore(0.1,0.2,0.2,0.2,0.2));
}

TEST(Mappers_TechSchoolTests, ArmyBonusImported)
{
	std::istringstream input("{\n\t\tarmy_tech_research_bonus=0.1}");
	mappers::TechSchool newTechSchool(input);
	ASSERT_GT(0.0001, abs(newTechSchool.calculateComparisonScore(0.3,0.2,0.2,0.2,0.2)));
}

TEST(Mappers_TechSchoolTests, IncreasingCommerceInvestmentIncreasesScore)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_LT(0, newTechSchool.calculateComparisonScore(0.2,0.3,0.2,0.2,0.2));
}

TEST(Mappers_TechSchoolTests, DecreasingCommerceInvestmentIncreasesScore)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_LT(0, newTechSchool.calculateComparisonScore(0.2,0.1,0.2,0.2,0.2));
}

TEST(Mappers_TechSchoolTests, CommerceBonusImported)
{
	std::istringstream input("{\n\t\tcommerce_tech_research_bonus=0.1}");
	mappers::TechSchool newTechSchool(input);
	ASSERT_GT(0.0001, abs(newTechSchool.calculateComparisonScore(0.2,0.3,0.2,0.2,0.2)));
}

TEST(Mappers_TechSchoolTests, IncreasingCultureInvestmentIncreasesScore)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_LT(0, newTechSchool.calculateComparisonScore(0.2,0.2,0.3,0.2,0.2));
}

TEST(Mappers_TechSchoolTests, DecreasingCultureInvestmentIncreasesScore)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_LT(0, newTechSchool.calculateComparisonScore(0.2,0.2,0.1,0.2,0.2));
}

TEST(Mappers_TechSchoolTests, CultureBonusImported)
{
	std::istringstream input("{\n\t\tculture_tech_research_bonus=0.1}");
	mappers::TechSchool newTechSchool(input);
	ASSERT_GT(0.0001, abs(newTechSchool.calculateComparisonScore(0.2,0.2,0.3,0.2,0.2)));
}

TEST(Mappers_TechSchoolTests, IncreasingIndustryInvestmentIncreasesScore)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_LT(0, newTechSchool.calculateComparisonScore(0.2,0.2,0.2,0.3,0.2));
}

TEST(Mappers_TechSchoolTests, DecreasingIndustryInvestmentIncreasesScore)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_LT(0, newTechSchool.calculateComparisonScore(0.2,0.2,0.2,0.1,0.2));
}

TEST(Mappers_TechSchoolTests, IndustryBonusImported)
{
	std::istringstream input("{\n\t\tindustry_tech_research_bonus=0.1}");
	mappers::TechSchool newTechSchool(input);
	ASSERT_GT(0.0001, abs(newTechSchool.calculateComparisonScore(0.2,0.2,0.2,0.3,0.2)));
}

TEST(Mappers_TechSchoolTests, IncreasingNavyInvestmentIncreasesScore)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_LT(0, newTechSchool.calculateComparisonScore(0.2,0.2,0.2,0.2,0.3));
}

TEST(Mappers_TechSchoolTests, DecreasingNavyInvestmentIncreasesScore)
{
	std::istringstream input("");
	mappers::TechSchool newTechSchool(input);
	ASSERT_LT(0, newTechSchool.calculateComparisonScore(0.2,0.2,0.2,0.2,0.1));
}

TEST(Mappers_TechSchoolTests, NavyBonusImported)
{
	std::istringstream input("{\n\t\tnavy_tech_research_bonus=0.1}");
	mappers::TechSchool newTechSchool(input);
	ASSERT_GT(0.0001, abs(newTechSchool.calculateComparisonScore(0.2,0.2,0.2,0.2,0.3)));
}