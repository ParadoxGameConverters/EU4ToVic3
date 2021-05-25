#include "ConverterVersion/ConverterVersion.h"
#include "gtest/gtest.h"

TEST(Mappers_ConverterVersionTests, PrimitivesDefaultToEmpty)
{
	std::stringstream input;
	mappers::ConverterVersion version;
	version.loadVersion(input);

	EXPECT_TRUE(version.getName().empty());
	EXPECT_TRUE(version.getDescription().empty());
	EXPECT_TRUE(version.getVersion().empty());
	EXPECT_EQ(GameVersion(0, 0, 0, 0), version.getMinimalVersion());
}

TEST(Mappers_ConverterVersionTests, PrimitivesCanBeSet)
{
	std::stringstream input;
	input << "name = \"Test Name\"";
	input << "version = \"5.4-Test\"";
	input << "descriptionLine = \"test description\"";
	input << "minimalIncomingSave = \"2.4.5\"";
	mappers::ConverterVersion version;
	version.loadVersion(input);

	EXPECT_EQ("Test Name", version.getName());
	EXPECT_EQ("5.4-Test", version.getVersion());
	EXPECT_EQ("test description", version.getDescription());
	EXPECT_EQ(GameVersion(2, 4, 5, 0), version.getMinimalVersion());
}
