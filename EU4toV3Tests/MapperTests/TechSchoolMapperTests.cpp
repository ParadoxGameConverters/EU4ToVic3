#include "gtest/gtest.h"
#include "../EU4toV3/Source/Mappers/TechSchools/TechSchoolMapper.h"



TEST(Mappers_TechSchoolMapperTests, DefaultSchoolIsTraditionalAcademic)
{
	std::stringstream emptyInput;
	mappers::TechSchoolMapper newTechSchools(emptyInput);

	ASSERT_EQ(std::string("traditional_academic"), newTechSchools.findBestTechSchool(0.2, 0.2, 0.2, 0.2, 0.2));
}


TEST(Mappers_TechSchoolMapperTests, InvestmentchangesTechSchool)
{
	std::stringstream emptyInput;
	std::stringstream techSchoolsInput("\
	schools = {\
		traditional_academic = \
		{\
			army_tech_research_bonus = 0\
			commerce_tech_research_bonus = 0\
			culture_tech_research_bonus = 0\
			industry_tech_research_bonus = 0\
			navy_tech_research_bonus = 0\
		}\
		army_tech_school = \
		{\
			army_tech_research_bonus = 0.15\
			commerce_tech_research_bonus = 0\
			culture_tech_research_bonus = -0.1\
			industry_tech_research_bonus = 0.1\
			navy_tech_research_bonus = -0.05\
			unciv_economic_modifier = -0.05\
			unciv_military_modifier = -0.15\
	}}");
	mappers::TechSchoolMapper newTechSchools(techSchoolsInput);

	ASSERT_NE(std::string("traditional_academic"), newTechSchools.findBestTechSchool(0.7, 0.1, 0.1, 0.1, 0.1));
}


TEST(Mappers_TechSchoolMapperTests, NoInvestmentDoesntCrash)
{
	std::stringstream techSchoolsInput("schools = {\
	traditional_academic = \
	{\
		army_tech_research_bonus = 0\
			commerce_tech_research_bonus = 0\
			culture_tech_research_bonus = 0\
			industry_tech_research_bonus = 0\
			navy_tech_research_bonus = 0\
	}\
	army_tech_school = \
	{\
		army_tech_research_bonus = 0.15\
		commerce_tech_research_bonus = 0\
		culture_tech_research_bonus = -0.1\
		industry_tech_research_bonus = 0.1\
		navy_tech_research_bonus = -0.05\
		unciv_economic_modifier = -0.05\
		unciv_military_modifier = -0.15\
	}}");
	mappers::TechSchoolMapper newTechSchools(techSchoolsInput);

	ASSERT_NO_THROW(const auto& bestschool = newTechSchools.findBestTechSchool(0.0, 0.0, 0.0, 0.0, 0.0));
}