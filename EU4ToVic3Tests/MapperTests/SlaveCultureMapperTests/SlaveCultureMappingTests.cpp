#include "SlaveCultureMapper/SlaveCultureMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_SlaveCultureMappingTests, DefaultsDefaultToDefaults)
{

	std::stringstream input;
	const mappers::SlaveCultureMapping mapping(input);

	EXPECT_FALSE(mapping.getMasterHeritage());
	EXPECT_FALSE(mapping.getMasterLanguage());
	EXPECT_TRUE(mapping.getSlaveCulture().empty());
}

TEST(Mappers_SlaveCultureMappingTests, RulesCanBeLoaded)
{
	std::stringstream input;
	input << "masterHeritage = trait masterLanguage = trait2 slave = culture";
	const mappers::SlaveCultureMapping mapping(input);

	EXPECT_EQ("trait", mapping.getMasterHeritage());
	EXPECT_EQ("trait2", mapping.getMasterLanguage());
	EXPECT_EQ("culture", mapping.getSlaveCulture());
}
