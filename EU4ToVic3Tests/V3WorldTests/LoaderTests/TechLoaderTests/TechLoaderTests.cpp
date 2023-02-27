#include "Loaders/TechLoader/TechLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_TechLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::TechLoader loader;

	EXPECT_TRUE(loader.getTechs().empty());
}

TEST(V3World_TechLoaderTests, TechsCanBeLoaded)
{
	V3::TechLoader loader;
	loader.loadTechs(modFS);

	ASSERT_EQ(4, loader.getTechs().size());
	EXPECT_TRUE(loader.getTechs().contains("tech_1"));
	EXPECT_TRUE(loader.getTechs().contains("tech_2"));
	EXPECT_TRUE(loader.getTechs().contains("tech_3"));
	EXPECT_TRUE(loader.getTechs().contains("tech_4"));
}
