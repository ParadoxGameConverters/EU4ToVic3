#include "ReligionMapper/ReligionDefinitionLoader/ReligionDefinitionLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});

TEST(Mappers_ReligionDefinitionLoaderTests, religionsCanBeRetrieved)
{
	mappers::ReligionDefinitionLoader loader;
	loader.loadDefinitions(modFS);

	const auto& religion = loader.getReligionDef("religion");
	ASSERT_TRUE(religion);

	EXPECT_EQ("religion", religion->name);
	EXPECT_EQ("gfx/interface/icons/religion_icons/religion.dds", religion->texture);
	EXPECT_THAT(religion->traits, testing::UnorderedElementsAre("religiontrait"));
	EXPECT_EQ(commonItems::Color(std::array{1, 2, 3}), *religion->color);
	EXPECT_TRUE(religion->taboos.empty());
}

TEST(Mappers_ReligionDefinitionLoaderTests, MismatchReturnsNullopt)
{
	mappers::ReligionDefinitionLoader loader;
	loader.loadDefinitions(modFS);

	const auto& religion = loader.getReligionDef("nonsense");
	ASSERT_FALSE(religion);
}
