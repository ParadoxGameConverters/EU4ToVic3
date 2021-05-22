#include "Wars/EU4War.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4WarTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const EU4::War war(input);

	ASSERT_TRUE(war.getAttackers().empty());
	ASSERT_TRUE(war.getDefenders().empty());
	ASSERT_TRUE(war.getName().empty());
	ASSERT_FALSE(war.getDetails().startDate.isSet());
	ASSERT_EQ(0, war.getDetails().targetProvinceID);
	ASSERT_TRUE(war.getDetails().targetTag.empty());
	ASSERT_TRUE(war.getDetails().warGoalClass.empty());
	ASSERT_TRUE(war.getDetails().warGoalType.empty());
}

TEST(EU4World_EU4WarTests, warCanBeLoaded)
{
	std::stringstream input;
	input << "name = \"Silly War\"";
	input << "history = {\n";
	input << "\t9.9.9 = { it begins }\n";
	input << "\t10.10.10 = { it doesn't end }\n";
	input << "}\n";
	input << "attackers = { ULM }\n";
	input << "defenders = { FRA TUR HAB }\n";
	input << "take_province = {\n";
	input << "\tprovince = 13\n";
	input << "\ttype = conquest\n";
	input << "\ttag = FRA\n";
	input << "}\n";
	const EU4::War war(input);

	ASSERT_EQ("ULM", war.getAttackers()[0]);
	ASSERT_EQ("FRA", war.getDefenders()[0]);
	ASSERT_EQ("TUR", war.getDefenders()[1]);
	ASSERT_EQ("HAB", war.getDefenders()[2]);
	ASSERT_EQ("Silly War", war.getName());
	ASSERT_EQ(date("9.9.9"), war.getDetails().startDate);
	ASSERT_EQ(13, war.getDetails().targetProvinceID);
	ASSERT_EQ("FRA", war.getDetails().targetTag);
	ASSERT_EQ("take_province", war.getDetails().warGoalClass);
	ASSERT_EQ("conquest", war.getDetails().warGoalType);
}
