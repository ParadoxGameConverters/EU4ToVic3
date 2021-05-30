#include "DefinitionScraper/DefinitionScraper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_DefinitionScraperTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(input);

	EXPECT_TRUE(definitions.getProvinceIDs().empty());
}

TEST(EU4World_DefinitionScraperTests, firstLineIsIgnored)
{
	std::stringstream input;
	input << "comment\n";
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(input);

	EXPECT_TRUE(definitions.getProvinceIDs().empty());
}

TEST(EU4World_DefinitionScraperTests, definitionsCanBeLoadedAndPinged)
{
	std::stringstream input;
	input << "comment\n";
	input << "1;1;2;3;name1;x;\n";
	input << "2;4;5;6;name2;x;\n";
	input << "3;7;8;9;name3;x;\n";
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(input);

	EXPECT_THAT(definitions.getProvinceIDs(), UnorderedElementsAre(1, 2, 3));
	EXPECT_TRUE(definitions.isValidID(1));
	EXPECT_TRUE(definitions.isValidID(2));
	EXPECT_TRUE(definitions.isValidID(3));
}

TEST(EU4World_DefinitionScraperTests, commentsAndEmptyLinesAreIgnored)
{
	std::stringstream input;
	input << "comment\n";
	input << "1;1;2;3;name1;x;\n";
	input << "# comment\n";
	input << "\n";
	input << "2;5;6;7;name2;x; # commentmore\n";
	input << "3;1;3;4;name3;x; # commentmore;x;x;1;2;3;x;x;\n";
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(input);

	EXPECT_THAT(definitions.getProvinceIDs(), UnorderedElementsAre(1, 2, 3));
}

TEST(EU4World_DefinitionScraperTests, incompleteRGBDefinitionsAreIgnored)
{
	std::stringstream input;
	input << "comment\n";
	input << "1;1;2;3;name1;x;\n";
	input << "2;4;;6;name2;x;\n";
	input << "3;7;8;;name3;x;\n";
	input << "4;;8;9;name3;x;\n";
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(input);

	EXPECT_THAT(definitions.getProvinceIDs(), UnorderedElementsAre(1));
	EXPECT_TRUE(definitions.isValidID(1));
	EXPECT_FALSE(definitions.isValidID(2));
	EXPECT_FALSE(definitions.isValidID(3));
	EXPECT_FALSE(definitions.isValidID(4));
}

TEST(EU4World_DefinitionScraperTests, randomJunkIsIgnored)
{
	std::stringstream input;
	input << "comment\n";
	input << "1;junk;2;3;;x;\n";
	input << "2\n";
	input << "johnnyboy\n";
	input << "3;7;8;9;name3;x;\n";
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(input);

	EXPECT_THAT(definitions.getProvinceIDs(), UnorderedElementsAre(3));
	EXPECT_FALSE(definitions.isValidID(1));
	EXPECT_FALSE(definitions.isValidID(2));
	EXPECT_TRUE(definitions.isValidID(3));
}

TEST(EU4World_DefinitionScraperTests, doubleEntriesAreIgnored)
{
	std::stringstream input;
	input << "comment\n";
	input << "3;7;8;9;name3;x;\n";
	input << "3;7;8;9;name3;x;\n";
	input << "3;7;8;9;name3;x;\n";
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(input);

	EXPECT_THAT(definitions.getProvinceIDs(), UnorderedElementsAre(3));
	EXPECT_FALSE(definitions.isValidID(1));
	EXPECT_FALSE(definitions.isValidID(2));
	EXPECT_TRUE(definitions.isValidID(3));
}
