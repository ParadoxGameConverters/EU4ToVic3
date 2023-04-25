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

	ASSERT_TRUE(loader.getDefinitions().contains("a_heritage"));
	ASSERT_TRUE(loader.getDefinitions().contains("a_heritage_2"));
	ASSERT_TRUE(loader.getDefinitions().contains("not_heritage"));
	ASSERT_TRUE(loader.getDefinitions().contains("not_heritage_2"));

	EXPECT_EQ("a_heritage", loader.getDefinitions().at("a_heritage").name);
	EXPECT_TRUE(loader.getDefinitions().at("a_heritage").isHeritageTrait);
	EXPECT_EQ("a_heritage_2", loader.getDefinitions().at("a_heritage_2").name);
	EXPECT_TRUE(loader.getDefinitions().at("a_heritage_2").isHeritageTrait);
	EXPECT_EQ("not_heritage", loader.getDefinitions().at("not_heritage").name);
	EXPECT_FALSE(loader.getDefinitions().at("not_heritage").isHeritageTrait);
	EXPECT_EQ("not_heritage_2", loader.getDefinitions().at("not_heritage_2").name);
	EXPECT_FALSE(loader.getDefinitions().at("not_heritage_2").isHeritageTrait);
}
