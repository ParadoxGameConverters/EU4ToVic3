#include "EmpireParser/EmpireParser.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_EmpireParserTests, emperorDefaultsToBlank)
{
	std::stringstream input;
	const EU4::EmpireParser empire(input);

	EXPECT_TRUE(empire.getEmperor().empty());
}

TEST(EU4World_EmpireParserTests, emperorCanBeLoaded)
{
	std::stringstream input;
	input << "emperor = HAB\n";
	const EU4::EmpireParser empire(input);

	EXPECT_EQ("HAB", empire.getEmperor());
}

TEST(EU4World_EmpireParserTests, refomsDefaultToBlank)
{
	std::stringstream input;
	const EU4::EmpireParser empire(input);

	EXPECT_TRUE(empire.getHREReforms().empty());
}

TEST(EU4World_EmpireParserTests, reformsCanBeLoaded)
{
	std::stringstream input;
	input << "passed_reform=reform1\n";
	input << "passed_reform=reform2\n";
	const EU4::EmpireParser empire(input);

	EXPECT_THAT(empire.getHREReforms(), UnorderedElementsAre("reform1", "reform2"));
}
