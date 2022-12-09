#include "CultureMapper/CultureTraitMapper/CultureTraitMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_CultureTraitMappingTests, primitivesDefaultsToBlank)
{
	std::stringstream input;
	const mappers::CultureTraitMapping mapping(input);

	EXPECT_TRUE(mapping.getCulture().empty());
	EXPECT_TRUE(mapping.getCultureGroup().empty());
	EXPECT_TRUE(mapping.getTraits().empty());
	EXPECT_TRUE(mapping.getEthnicity().empty());
	EXPECT_TRUE(mapping.getGraphics().empty());
}

TEST(Mappers_CultureTraitMappingTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "eu4 = culture1 eu4group = group1 trait = testtrait trait = testtrait2 ethnicity = testable graphics = something";
	const mappers::CultureTraitMapping mapping(input);

	EXPECT_EQ("culture1", mapping.getCulture());
	EXPECT_EQ("group1", mapping.getCultureGroup());
	EXPECT_THAT(mapping.getTraits(), testing::UnorderedElementsAre("testtrait", "testtrait2"));
	EXPECT_EQ("testable", mapping.getEthnicity());
	EXPECT_EQ("something", mapping.getGraphics());
}
