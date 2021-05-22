#include "Titles/TitleMapping.h"
#include "gtest/gtest.h"

TEST(Mappers_TitleMappingTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const mappers::TitleMapping mapping(input);

	ASSERT_TRUE(mapping.getName().empty());
	ASSERT_TRUE(mapping.getTitle().empty());
	ASSERT_FALSE(mapping.hasIndianRegion());
	ASSERT_FALSE(mapping.hasIslamicRegion());
}

TEST(Mappers_TitleMappingTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "name = alampur\n";
	input << "title = c_alampur\n";
	const mappers::TitleMapping mapping(input);

	ASSERT_EQ("alampur", mapping.getName());
	ASSERT_EQ("c_alampur", mapping.getTitle());
	ASSERT_FALSE(mapping.hasIslamicRegion());
	ASSERT_FALSE(mapping.hasIslamicRegion());
}

TEST(Mappers_TitleMappingTests, namesAreLowercased)
{
	std::stringstream input;
	input << "name = Alampur\n";
	const mappers::TitleMapping mapping(input);

	ASSERT_EQ("alampur", mapping.getName());
}

TEST(Mappers_TitleMappingTests, indianRegionCanBeMatched)
{
	std::stringstream input;
	input << "name = alampur\n";
	input << "title = c_alampur\n";
	input << "region = e_deccan\n";
	const mappers::TitleMapping mapping(input);

	ASSERT_TRUE(mapping.hasIndianRegion());
	ASSERT_FALSE(mapping.hasIslamicRegion());
}

TEST(Mappers_TitleMappingTests, islamicRegionCanBeMatched)
{
	std::stringstream input;
	input << "name = Alamut\n";
	input << "title = c_alamut\n";
	input << "region = e_persia\n";
	const mappers::TitleMapping mapping(input);

	ASSERT_EQ("alamut", mapping.getName());
	ASSERT_EQ("c_alamut", mapping.getTitle());
	ASSERT_FALSE(mapping.hasIndianRegion());
	ASSERT_TRUE(mapping.hasIslamicRegion());
}
