#include "CountryManager/EU4Country.h"
#include "Loaders/NationalBudgetLoader/Script/AddScript.h"
#include "Loaders/NationalBudgetLoader/Script/MultiplyScript.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"


TEST(V3World_MathScriptTests, MathScriptTriggersAreRecognized)
{
	std::stringstream lawInput;
	std::stringstream colonyInput;
	std::stringstream GPInput;
	std::stringstream invalidInput;

	lawInput << "\tvic3_law = law_serfdom";
	colonyInput << "\tis_colony = yes";
	invalidInput << "\tis_awesome = yes";

	V3::MathScript law;
	law.loadMathScript(lawInput);
	V3::MathScript colony;
	colony.loadMathScript(colonyInput);
	V3::MathScript invalid;
	invalid.loadMathScript(invalidInput);

	V3::Country country;
	country.setSourceCountry(std::make_shared<EU4::Country>());

	EXPECT_FALSE(law.isValid(country));
	EXPECT_FALSE(colony.isValid(country));
	EXPECT_FALSE(invalid.isValid(country));


	V3::ProcessedData data;
	data.laws.emplace("law_serfdom");

	std::stringstream eu4Input;
	eu4Input << "colonial_parent = AAA";

	auto sourceCountry = std::make_shared<EU4::Country>("TAG", eu4Input);

	country.setProcessedData(data);
	country.setSourceCountry(sourceCountry);


	EXPECT_TRUE(law.isValid(country));
	EXPECT_TRUE(colony.isValid(country));
}

TEST(V3World_AddScriptTests, AddScriptsAdd)
{
	std::stringstream input0;
	std::stringstream input1;

	input0 << "\tvalue = 3";
	input1 << "\tvalue = 2";

	V3::AddScript script0;
	V3::AddScript script1;

	script0.loadMathScript(input0);
	script1.loadMathScript(input1);

	EXPECT_DOUBLE_EQ(4, script0 + 1);
	EXPECT_DOUBLE_EQ(3, script1 + 1);
	EXPECT_DOUBLE_EQ(6, V3::AddScript::combine({script0, script1}) + 1);
}

TEST(V3World_MultiplyScriptTests, MultiplyScriptsMultiplyAfterAdditivelyCombining)
{
	std::stringstream input0;
	std::stringstream input1;

	input0 << "\tvalue = 1.5";
	input1 << "\tvalue = 2";

	V3::MultiplyScript script0;
	V3::MultiplyScript script1;

	script0.loadMathScript(input0);
	script1.loadMathScript(input1);

	EXPECT_DOUBLE_EQ(3, script0 * 2);
	EXPECT_DOUBLE_EQ(4, script1 * 2);
	EXPECT_DOUBLE_EQ(5, V3::MultiplyScript::combine({script0, script1}) * 2);
}