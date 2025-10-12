#include "CultureMapper/TraitDefinitionLoader/TraitDefinitionLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game", {});
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
	ASSERT_TRUE(loader.getDefinitions().contains("language_trait_1"));
	ASSERT_TRUE(loader.getDefinitions().contains("tradition_trait_2"));
	ASSERT_TRUE(loader.getDefinitions().contains("descriptive_trait_1"));

	EXPECT_EQ("heritage_trait_1", loader.getDefinitions().at("heritage_trait_1").name);
	EXPECT_TRUE(loader.getDefinitions().at("heritage_trait_1").isHeritage);
	EXPECT_EQ("heritage_trait_2", loader.getDefinitions().at("heritage_trait_2").name);
	EXPECT_TRUE(loader.getDefinitions().at("heritage_trait_2").isHeritage);
	EXPECT_EQ("language_trait_1", loader.getDefinitions().at("language_trait_1").name);
	EXPECT_TRUE(loader.getDefinitions().at("language_trait_1").isLanguage);
	EXPECT_EQ("tradition_trait_2", loader.getDefinitions().at("tradition_trait_2").name);
	EXPECT_TRUE(loader.getDefinitions().at("tradition_trait_2").isTradition);
	EXPECT_EQ("descriptive_trait_1", loader.getDefinitions().at("descriptive_trait_1").name);
	EXPECT_FALSE(loader.getDefinitions().at("descriptive_trait_1").isTradition);
	EXPECT_FALSE(loader.getDefinitions().at("descriptive_trait_1").isLanguage);
	EXPECT_FALSE(loader.getDefinitions().at("descriptive_trait_1").isHeritage);
}
