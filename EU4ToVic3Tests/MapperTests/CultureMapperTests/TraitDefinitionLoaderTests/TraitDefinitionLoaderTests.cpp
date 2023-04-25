#include "CultureMapper/TraitDefinitionLoader/TraitDefinitionLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(Mappers_TraitDefinitionLoaderTests, LoaderDefaultsToDry)
{
	const mappers::TraitDefinitionLoader loader;
	ASSERT_TRUE(loader.getDefinitions().empty());
}

TEST(Mappers_TraitDefinitionLoaderTests, TraitsCanBeRetrieved)
{
	mappers::TraitDefinitionLoader loader;
	loader.loadDefinitions(modFS);

	ASSERT_TRUE(loader.getDefinitions().contains("heritage_trait_1"));
	ASSERT_TRUE(loader.getDefinitions().contains("heritage_trait_2"));
	ASSERT_TRUE(loader.getDefinitions().contains("nonheritage_trait_1"));
	ASSERT_TRUE(loader.getDefinitions().contains("nonheritage_trait_2"));

	EXPECT_EQ("heritage_trait_1", loader.getDefinitions().at("heritage_trait_1").name);
	EXPECT_TRUE(loader.getDefinitions().at("heritage_trait_1").isHeritageTrait);
	EXPECT_EQ("heritage_trait_2", loader.getDefinitions().at("heritage_trait_2").name);
	EXPECT_TRUE(loader.getDefinitions().at("heritage_trait_2").isHeritageTrait);
	EXPECT_EQ("nonheritage_trait_1", loader.getDefinitions().at("nonheritage_trait_1").name);
	EXPECT_FALSE(loader.getDefinitions().at("nonheritage_trait_1").isHeritageTrait);
	EXPECT_EQ("nonheritage_trait_2", loader.getDefinitions().at("nonheritage_trait_2").name);
	EXPECT_FALSE(loader.getDefinitions().at("nonheritage_trait_2").isHeritageTrait);
}
