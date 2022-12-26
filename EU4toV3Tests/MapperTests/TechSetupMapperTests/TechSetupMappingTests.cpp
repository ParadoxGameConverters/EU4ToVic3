#include "TechSetupMapper/TechSetupMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_TechSetupMappingTests, DefaultsDefaultToDefaults)
{

	std::stringstream input;
	const mappers::TechSetupMapping mapping(input);

	EXPECT_TRUE(mapping.getTechScores().empty());
}

TEST(Mappers_TechSetupMappingTests, RulesCanBeLoaded)
{
	std::stringstream input;
	input << "enclosure = 17 manufacturies = 27 shaft_mining = 33\n";
	const mappers::TechSetupMapping mapping(input);

	EXPECT_THAT(mapping.getTechScores(),
		 testing::UnorderedElementsAre(std::pair("enclosure", 17), std::pair("manufacturies", 27), std::pair("shaft_mining", 33)));
}
