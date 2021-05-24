#include "ConverterVersion/ConverterVersion.h"
#include "gtest/gtest.h"

TEST(Mappers_ConverterVersionTests, PrimitivesDefaultToEmpty)
{
	std::stringstream input;
	mappers::ConverterVersion version(input);

	ASSERT_TRUE(version.getName().empty());
	ASSERT_TRUE(version.getDescription().empty());
	ASSERT_TRUE(version.getVersion().empty());
	ASSERT_EQ(GameVersion(0, 0, 0, 0), version.getMinimalVersion());
}

TEST(Mappers_ConverterVersionTests, PrimitivesCanBeSet)
{
	std::stringstream input;
	input << "name = \"Test Name\"";
	input << "version = \"5.4-Test\"";
	input << "descriptionLine = \"test description\"";
	input << "minimalIncomingSave = \"2.4.5\"";
	mappers::ConverterVersion version(input);

	ASSERT_EQ("Test Name", version.getName());
	ASSERT_EQ("5.4-Test", version.getVersion());
	ASSERT_EQ("test description", version.getDescription());
	ASSERT_EQ(GameVersion(2, 4, 5, 0), version.getMinimalVersion());
}
