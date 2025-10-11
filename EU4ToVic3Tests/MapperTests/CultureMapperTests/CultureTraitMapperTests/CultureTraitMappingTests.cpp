#include "CultureMapper/CultureTraitMapper/CultureTraitMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_CultureTraitMappingTests, primitivesDefaultsToBlank)
{
	std::stringstream input;
	const mappers::CultureTraitMapping mapping(input);

	EXPECT_TRUE(mapping.getCulture().empty());
	EXPECT_TRUE(mapping.getCultureGroup().empty());
	EXPECT_TRUE(mapping.getLanguage().empty());
	EXPECT_TRUE(mapping.getHeritage().empty());
	EXPECT_TRUE(mapping.getTraditions().empty());
	EXPECT_TRUE(mapping.getEthnicity().empty());
	EXPECT_TRUE(mapping.getGraphics().empty());
}

TEST(Mappers_CultureTraitMappingTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "eu4 = culture1 eu4group = group1 heritage = testtrait language = testtrait2 tradition = testtrait3 tradition = testtrait4 ethnicity = testable "
				"graphics = something";
	const mappers::CultureTraitMapping mapping(input);

	EXPECT_EQ("culture1", mapping.getCulture());
	EXPECT_EQ("group1", mapping.getCultureGroup());
	EXPECT_EQ("testtrait", mapping.getHeritage());
	EXPECT_EQ("testtrait2", mapping.getLanguage());
	EXPECT_THAT(mapping.getTraditions(), testing::UnorderedElementsAre("testtrait3", "testtrait4"));
	EXPECT_EQ("testable", mapping.getEthnicity());
	EXPECT_EQ("something", mapping.getGraphics());
}
