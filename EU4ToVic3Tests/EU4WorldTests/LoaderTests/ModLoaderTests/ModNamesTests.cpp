#include "ModLoader/ModNames.h"
#include "gtest/gtest.h"

TEST(EU4World_ModNamesTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::ModNames theMod(input);

	EXPECT_TRUE(theMod.getName().empty());
	EXPECT_TRUE(theMod.getPath().empty());
}

TEST(EU4World_ModNamesTests, primitivesCanBeSet)
{
	std::stringstream input;
	input << "name=\"modName\"\n";
	input << "filename=\"mod/modPath.mod\"\n";

	const EU4::ModNames theMod(input);
	EXPECT_EQ("modName", theMod.getName());
	EXPECT_EQ("mod/modPath.mod", theMod.getPath());
}
