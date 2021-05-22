#include "Relations/EU4Empire.h"
#include "gtest/gtest.h"

TEST(EU4World_Relations_EmpireTests, emperorDefaultsToBlank)
{
	std::stringstream input;
	const EU4::EU4Empire empire(input);

	ASSERT_TRUE(empire.getEmperor().empty());
}

TEST(EU4World_Relations_EmpireTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "emperor = HAB\n";
	const EU4::EU4Empire empire(input);

	ASSERT_EQ("HAB", empire.getEmperor());
}

TEST(EU4World_Relations_EmpireTests, refomsDefaultToBlank)
{
	std::stringstream input;
	const EU4::EU4Empire empire(input);

	ASSERT_TRUE(empire.getHREReforms().empty());
}

TEST(EU4World_Relations_EmpireTests, reformsCanBeLoaded)
{
	std::stringstream input;
	input << "passed_reform=emperor_geteilte_macht\n";
	const EU4::EU4Empire empire(input);

	ASSERT_TRUE(empire.getHREReforms().contains("emperor_geteilte_macht"));
}
