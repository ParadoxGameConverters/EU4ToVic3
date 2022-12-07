#include "CultureMapper/CultureDefinitionLoader/CultureDefinitionLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});

TEST(Mappers_CultureDefinitionLoaderTests, CulturesCanBeRetrieved)
{
	mappers::CultureDefinitionLoader loader;
	loader.loadDefinitions(modFS);

	const auto& vculture2 = loader.getCultureDef("vculture2");
	ASSERT_TRUE(vculture2);

	EXPECT_EQ("vculture2", vculture2->name);
	EXPECT_THAT(vculture2->ethnicities, testing::UnorderedElementsAre("ethnotest2"));
}

TEST(Mappers_CultureDefinitionLoaderTests, MismatchReturnsNullopt)
{
	mappers::CultureDefinitionLoader loader;
	loader.loadDefinitions(modFS);

	const auto& culture = loader.getCultureDef("nonsense");
	ASSERT_FALSE(culture);
}
