#include "CultureMapper/CultureDefinitionLoader/CultureDefinitionLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(Mappers_CultureDefinitionLoaderTests, LoaderDefaultsToDry)
{
	const mappers::CultureDefinitionLoader loader;
	ASSERT_TRUE(loader.getDefinitions().empty());
}

TEST(Mappers_CultureDefinitionLoaderTests, CulturesCanBeRetrieved)
{
	mappers::CultureDefinitionLoader loader;
	loader.loadDefinitions(modFS);

	ASSERT_TRUE(loader.getDefinitions().contains("vculture2"));
	const auto& vculture2 = loader.getDefinitions().at("vculture2");

	EXPECT_EQ("vculture2", vculture2.name);
	EXPECT_THAT(vculture2.ethnicities, testing::UnorderedElementsAre("ethnotest2"));
}
