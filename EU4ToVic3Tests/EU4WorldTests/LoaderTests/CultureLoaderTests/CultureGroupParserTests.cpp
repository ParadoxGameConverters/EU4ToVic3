#include "CultureLoader/CultureGroupParser.h"
#include "CultureLoader/CultureParser.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::ElementsAre;

TEST(EU4World_CultureGroupParserTests, nameCanBeSet)
{
	std::stringstream input;
	const EU4::CultureGroupParser group("name", input, {});

	EXPECT_EQ("name", group.getName());
}

TEST(EU4World_CultureGroupParserTests, culturesCanBeLoaded)
{
	std::stringstream input;
	input << "someculture = {}\n";
	input << "otherculture = {}\n";
	const EU4::CultureGroupParser group("name", input, {});

	EXPECT_EQ(2, group.getCultures().size());
	EXPECT_TRUE(group.getCultures().contains("someculture"));
	EXPECT_TRUE(group.getCultures().contains("otherculture"));
}

TEST(EU4World_CultureGroupParserTests, culturesCanBeSkipped)
{
	std::stringstream input;
	input << "someculture = {}\n";
	input << "otherculture = {}\n";
	const EU4::CultureGroupParser group("name", input, {"someculture"});

	EXPECT_EQ(1, group.getCultures().size());
	EXPECT_FALSE(group.getCultures().contains("someculture"));
	EXPECT_TRUE(group.getCultures().contains("otherculture"));
}

TEST(EU4World_CultureGroupParserTests, culturesCanBeMerged)
{
	std::stringstream input;
	input << "someculture = {\n";
	input << "\tmale_names = { bob }\n";
	input << "\tfemale_names = { boba }\n";
	input << "\tdynasty_names = { bobby }\n";
	input << "}\n";
	EU4::CultureGroupParser group("name", input, {});

	std::stringstream input2;
	input2 << "male_names = { jon }\n";
	input2 << "female_names = { jona }\n";
	input2 << "dynasty_names = { jonny }\n";
	const auto sameCulture = EU4::CultureParser(input2);

	group.mergeCulture("someculture", sameCulture);

	const auto& someCulture = group.getCultures().at("someculture");

	EXPECT_THAT(someCulture.getMaleNames(), ElementsAre("bob", "jon"));
	EXPECT_THAT(someCulture.getFemaleNames(), ElementsAre("boba", "jona"));
	EXPECT_THAT(someCulture.getDynastyNames(), ElementsAre("bobby", "jonny"));
}

TEST(EU4World_CultureGroupParserTests, unmergeableCulturesAreAddedToGroup)
{
	std::stringstream input;
	input << "someculture = {\n";
	input << "\tmale_names = { bob }\n";
	input << "\tfemale_names = { boba }\n";
	input << "\tdynasty_names = { bobby }\n";
	input << "}\n";
	EU4::CultureGroupParser group("name", input, {});

	std::stringstream input2;
	input2 << "male_names = { jon }\n";
	input2 << "female_names = { jona }\n";
	input2 << "dynasty_names = { jonny }\n";
	const auto otherCulture = EU4::CultureParser(input2);

	group.mergeCulture("otherculture", otherCulture);

	EXPECT_EQ(2, group.getCultures().size());
	EXPECT_TRUE(group.getCultures().contains("someculture"));
	EXPECT_TRUE(group.getCultures().contains("otherculture"));
}
