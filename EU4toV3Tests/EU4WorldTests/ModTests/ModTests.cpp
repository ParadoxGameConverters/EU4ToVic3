#include "Mods/Mod.h"
#include "gtest/gtest.h"

TEST(EU4World_ModTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::Mod theMod(input);

	ASSERT_TRUE(theMod.getName().empty());
	ASSERT_TRUE(theMod.getPath().empty());
}

TEST(EU4World_ModTests, primitivesCanBeSet)
{
	std::stringstream input;
	input << "name=modName\n";
	input << "path=modPath\n";

	const EU4::Mod theMod(input);
	ASSERT_EQ("modName", theMod.getName());
	ASSERT_EQ("modPath", theMod.getPath());
}

TEST(EU4World_ModTests, pathCanBeSetFromArchive)
{
	std::stringstream input;
	input << "archive=modPath\n";

	const EU4::Mod theMod(input);
	ASSERT_EQ("modPath", theMod.getPath());
}

TEST(EU4World_ModTests, modIsInvalidIfPathOrNameUnSet)
{
	std::stringstream input;
	EU4::Mod theMod(input);
	ASSERT_FALSE(theMod.isValid());

	std::stringstream input2;
	input2 << "name=modName\n";
	const EU4::Mod theMod2(input2);
	ASSERT_FALSE(theMod2.isValid());

	std::stringstream input3;
	input3 << "path=modPath\n";
	EU4::Mod theMod3(input3);
	ASSERT_FALSE(theMod3.isValid());
}

TEST(EU4World_ModTests, modIsValidIfNameAndPathSet)
{
	std::stringstream input;
	input << "name=modName\n";
	input << "path=modPath\n";

	const EU4::Mod theMod(input);
	ASSERT_TRUE(theMod.isValid());
}

TEST(EU4World_ModTests, modIsCompressedForZipAndBinPaths)
{
	std::stringstream input;
	input << "path=modPath\n";
	const EU4::Mod theMod(input);
	ASSERT_FALSE(theMod.isCompressed());

	std::stringstream input2;
	input2 << "path=modPath.zip\n";
	const EU4::Mod theMod2(input2);
	ASSERT_TRUE(theMod2.isCompressed());

	std::stringstream input3;
	input3 << "path=modPath.bin\n";
	const EU4::Mod theMod3(input3);
	ASSERT_TRUE(theMod3.isCompressed());
}