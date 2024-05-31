#include "CultureMapper/NewEU4CultureMapper/NewEU4CultureMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_NewEU4CultureMappingTests, primitivesDefaultsToBlank)
{
	std::stringstream input;
	const mappers::NewEU4CultureMapping mapping(input);

	EXPECT_TRUE(mapping.getCultures().empty());
	EXPECT_TRUE(mapping.getAddTraits().empty());
	EXPECT_TRUE(mapping.getRemoveTraits().empty());
}

TEST(Mappers_NewEU4CultureMappingTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "culture = cul1 culture = cul2 addtrait = add1 addtrait = add2 removetrait = rem1 removetrait = rem2";
	const mappers::NewEU4CultureMapping mapping(input);

	EXPECT_THAT(mapping.getCultures(), testing::UnorderedElementsAre("cul1", "cul2"));
	EXPECT_THAT(mapping.getAddTraits(), testing::UnorderedElementsAre("add1", "add2"));
	EXPECT_THAT(mapping.getRemoveTraits(), testing::UnorderedElementsAre("rem1", "rem2"));
}
