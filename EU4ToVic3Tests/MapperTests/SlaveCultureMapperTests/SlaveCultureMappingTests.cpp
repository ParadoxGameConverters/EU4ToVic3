#include "SlaveCultureMapper/SlaveCultureMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_SlaveCultureMappingTests, DefaultsDefaultToDefaults)
{

	std::stringstream input;
	const mappers::SlaveCultureMapping mapping(input);

	EXPECT_TRUE(mapping.getMasterTrait().empty());
	EXPECT_TRUE(mapping.getSlaveCulture().empty());
}

TEST(Mappers_SlaveCultureMappingTests, RulesCanBeLoaded)
{
	std::stringstream input;
	input << "master = trait slave = culture";
	const mappers::SlaveCultureMapping mapping(input);

	EXPECT_EQ("trait", mapping.getMasterTrait());
	EXPECT_EQ("culture", mapping.getSlaveCulture());
}
