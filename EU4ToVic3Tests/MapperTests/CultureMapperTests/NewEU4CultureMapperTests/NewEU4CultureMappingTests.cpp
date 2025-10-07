#include "CultureMapper/NewEU4CultureMapper/NewEU4CultureMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_NewEU4CultureMappingTests, primitivesDefaultsToBlank)
{
	std::stringstream input;
	const mappers::NewEU4CultureMapping mapping(input);

	EXPECT_TRUE(mapping.getCultures().empty());
	EXPECT_TRUE(mapping.getAddTraditions().empty());
	EXPECT_TRUE(mapping.getRemoveTraditions().empty());
}

TEST(Mappers_NewEU4CultureMappingTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "culture = cul1 culture = cul2 addtradition = add1 addtradition = add2 removetradition = rem1 removetradition = rem2 replacelanguage = "
				"new_language replaceheritage = new_heritage";
	const mappers::NewEU4CultureMapping mapping(input);

	EXPECT_THAT(mapping.getCultures(), testing::UnorderedElementsAre("cul1", "cul2"));
	EXPECT_THAT(mapping.getAddTraditions(), testing::UnorderedElementsAre("add1", "add2"));
	EXPECT_THAT(mapping.getRemoveTraditions(), testing::UnorderedElementsAre("rem1", "rem2"));
	EXPECT_EQ("new_heritage", *mapping.getReplaceHeritage());
	EXPECT_EQ("new_language", *mapping.getReplaceLanguage());
}
