#include "CultureGroups/Culture.h"
#include "CultureGroups/CultureGroup.h"
#include "gtest/gtest.h"

TEST(Mappers_CultureGroupTests, nameCanBeSet)
{
	std::stringstream input;
	const mappers::CultureGroup group("name", input);

	ASSERT_EQ("name", group.getName());
}

TEST(Mappers_CultureGroupTests, culturesCanBeLoaded)
{
	std::stringstream input;
	input << "someculture = {}\n";
	input << "otherculture = {}\n";
	const mappers::CultureGroup group("name", input);

	ASSERT_EQ(2, group.getCultures().size());
	ASSERT_TRUE(group.getCultures().contains("someculture"));
	ASSERT_TRUE(group.getCultures().contains("otherculture"));
}

TEST(Mappers_CultureGroupTests, culturesCanBePinged)
{
	std::stringstream input;
	input << "someculture = {}\n";
	input << "otherculture = {}\n";
	const mappers::CultureGroup group("name", input);

	ASSERT_TRUE(group.containsCulture("someculture"));
	ASSERT_TRUE(group.containsCulture("otherculture"));
}

TEST(Mappers_CultureGroupTests, culturesPingMismatchReturnsFalse)
{
	std::stringstream input;
	input << "someculture = {}\n";
	input << "otherculture = {}\n";
	const mappers::CultureGroup group("name", input);

	ASSERT_FALSE(group.containsCulture("missingculture"));
}

TEST(Mappers_CultureGroupTests, culturesCanBeMerged)
{
	std::stringstream input;
	input << "someculture = {\n";
	input << "\tmale_names = { bob }\n";
	input << "\tfemale_names = { boba }\n";
	input << "\tdynasty_names = { bobby }\n";
	input << "}\n";
	mappers::CultureGroup group("name", input);

	std::stringstream input2;
	input2 << "male_names = { jon }\n";
	input2 << "female_names = { jona }\n";
	input2 << "dynasty_names = { jonny }\n";
	const auto sameCulture = std::make_shared<mappers::Culture>(input2);

	group.mergeCulture("someculture", sameCulture);

	const auto someCulture = group.getCultures().find("someculture")->second;

	ASSERT_EQ(2, someCulture->getMaleNames().size());
	ASSERT_EQ("bob", someCulture->getMaleNames()[0]);
	ASSERT_EQ("jon", someCulture->getMaleNames()[1]);
	ASSERT_EQ(2, someCulture->getFemaleNames().size());
	ASSERT_EQ("boba", someCulture->getFemaleNames()[0]);
	ASSERT_EQ("jona", someCulture->getFemaleNames()[1]);
	ASSERT_EQ(2, someCulture->getDynastyNames().size());
	ASSERT_EQ("bobby", someCulture->getDynastyNames()[0]);
	ASSERT_EQ("jonny", someCulture->getDynastyNames()[1]);
}

TEST(Mappers_CultureGroupTests, unmergeableCulturesAreAddedToGroup)
{
	std::stringstream input;
	input << "someculture = {\n";
	input << "\tmale_names = { bob }\n";
	input << "\tfemale_names = { boba }\n";
	input << "\tdynasty_names = { bobby }\n";
	input << "}\n";
	mappers::CultureGroup group("name", input);

	std::stringstream input2;
	input2 << "male_names = { jon }\n";
	input2 << "female_names = { jona }\n";
	input2 << "dynasty_names = { jonny }\n";
	const auto otherCulture = std::make_shared<mappers::Culture>(input2);

	group.mergeCulture("otherculture", otherCulture);

	ASSERT_EQ(2, group.getCultures().size());
	ASSERT_TRUE(group.containsCulture("someculture"));
	ASSERT_TRUE(group.containsCulture("otherculture"));
}

TEST(Mappers_CultureGroupTests, neoCultureCanBeAdded)
{
	std::stringstream input;
	mappers::CultureGroup group("name", input);

	std::stringstream input2;
	input2 << "male_names = { jon }\n";
	input2 << "female_names = { jona }\n";
	input2 << "dynasty_names = { jonny }\n";
	const auto neoCulture = std::make_shared<mappers::Culture>(input2);

	group.addNeoCulture("neoCulture", neoCulture, "oldCulture");

	ASSERT_TRUE(group.containsCulture("neoCulture"));

	const auto& theNeoCulture = group.getCultures().find("neoCulture")->second;

	ASSERT_TRUE(theNeoCulture->isNeoCulture());
	ASSERT_EQ("oldCulture", theNeoCulture->getOriginalCulture());
	ASSERT_EQ(10, theNeoCulture->getRadicalism());
}

TEST(Mappers_CultureGroupTests, v2CultureGroupCanBeOutput)
{
	std::stringstream input;
	input << "leader = european\n";
	input << "is_overseas = yes\n";
	input << "unit = MiddleEasternGC\n";
	input << "union = TAG\n";
	input << "someculture = {\n";
	input << "\tprimary = TAG\n";
	input << "\tfirst_names = { Bob Jon }\n";
	input << "\tlast_names = { Bobby Johnny }\n";
	input << "\tradicalism = 4\n";
	input << "\tcolor = { 1 2 3 }\n";
	input << "}\n";
	mappers::CultureGroup group("someculturegroup", input);

	std::stringstream output;
	output << "\tleader = european\n";
	output << "\tis_overseas = yes\n";
	output << "\tunit = MiddleEasternGC\n";
	output << "\t\n";
	output << "\tsomeculture = {\n";
	output << "\t\tcolor = { 1 2 3 }\n";
	output << "\t\tradicalism = 4\n";
	output << "\t\tprimary = TAG\n";
	output << "\t\t\n";
	output << "\t\tfirst_names = { Bob Jon }\n";
	output << "\t\t\n";
	output << "\t\tlast_names = { Bobby Johnny }\n";
	output << "\t\t\n";
	output << "\t}\n";
	output << "\t\n";
	output << "\tunion = TAG\n";
	output << "\t\n";

	std::stringstream actual;
	actual << group;

	ASSERT_EQ(output.str(), actual.str());
}
