#include "Loaders/LawLoader/LawLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_LawLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::LawLoader loader;

	EXPECT_TRUE(loader.getLaws().empty());
}

TEST(V3World_LawLoaderTests, LawsCanBeLoaded)
{
	V3::LawLoader loader;
	loader.loadLaws(modFS);

	ASSERT_EQ(4, loader.getLaws().size());
	EXPECT_TRUE(loader.getLaws().contains("law_1"));
	EXPECT_TRUE(loader.getLaws().contains("law_2"));
	EXPECT_TRUE(loader.getLaws().contains("law_3"));
	EXPECT_TRUE(loader.getLaws().contains("law_4"));
}
