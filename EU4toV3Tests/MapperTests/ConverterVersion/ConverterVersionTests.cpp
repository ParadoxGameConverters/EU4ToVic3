#include "Mappers/ConverterVersion/ConverterVersion.h"
#include "gtest/gtest.h"

TEST(Mappers_ConverterVersionTests, PrimitivesDefaultToEmpty)
{
	std::stringstream input;
	mappers::ConverterVersion version;
	version.loadVersion(input);

	EXPECT_TRUE(version.getName().empty());
	EXPECT_TRUE(version.getVersion().empty());
}

TEST(Mappers_ConverterVersionTests, NameCanBeSet)
{
	std::stringstream input;
	input << "name = \"Test Name\"";
	mappers::ConverterVersion version;
	version.loadVersion(input);

	EXPECT_EQ("Test Name", version.getName());
}

TEST(Mappers_ConverterVersionTests, VersionCanBeSet)
{
	std::stringstream input;
	input << "version = \"5.4-Test\"";
	mappers::ConverterVersion version;
	version.loadVersion(input);

	EXPECT_EQ("5.4-Test", version.getVersion());
}

TEST(Mappers_ConverterVersionTests, DescriptionCanBeConstructed)
{
	std::stringstream input;
	input << "source = \"EU4\"\n";
	input << "target = \"Vic3\"\n";
	input << "minSource = \"1.31\"\n";
	input << "maxSource = \"1.31.5\"\n";
	input << "minTarget = \"1.0\"\n";
	input << "maxTarget = \"1.1\"\n";
	mappers::ConverterVersion version;
	version.loadVersion(input);

	EXPECT_EQ("Compatible with EU4 [v1.31-v1.31.5] and Vic3 [v1.0-v1.1]", version.getDescription());
}

TEST(Mappers_ConverterVersionTests, DescriptionDoesNotDuplicateVersions)
{
	std::stringstream input;
	input << "source = \"EU4\"\n";
	input << "target = \"Vic3\"\n";
	input << "minSource = \"1.31\"\n";
	input << "maxSource = \"1.31\"\n";
	input << "minTarget = \"1.0\"\n";
	input << "maxTarget = \"1.0\"\n";
	mappers::ConverterVersion version;
	version.loadVersion(input);

	EXPECT_EQ("Compatible with EU4 [v1.31] and Vic3 [v1.0]", version.getDescription());
}