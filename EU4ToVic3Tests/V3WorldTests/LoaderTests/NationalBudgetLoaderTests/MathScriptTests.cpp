#include "CountryManager/EU4Country.h"
#include "Loaders/NationalBudgetLoader/Script/AddScript.h"
#include "Loaders/NationalBudgetLoader/Script/MultiplyScript.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"


TEST(V3World_MathScriptTests, MathScriptTriggersAreRecognized)
{
	std::stringstream lawInput;
	std::stringstream techInput;
	std::stringstream colonyInput;
	std::stringstream tcInput;
	std::stringstream GPInput;
	std::stringstream industryInput;
	std::stringstream invalidInput;

	lawInput << "\tvic3_law = law_serfdom";
	techInput << "\tvic3_tech = centralization";
	colonyInput << "\tis_colony = yes";
	tcInput << "\tis_trade_company = yes";
	GPInput << "\tis_eu4_gp = yes";
	industryInput << "\tindustry_score_less_than = 2";
	invalidInput << "\tis_awesome = yes";

	V3::MathScript law;
	law.loadMathScript(lawInput);
	V3::MathScript tech;
	tech.loadMathScript(techInput);
	V3::MathScript colony;
	colony.loadMathScript(colonyInput);
	V3::MathScript tc;
	tc.loadMathScript(tcInput);
	V3::MathScript gp;
	gp.loadMathScript(GPInput);
	V3::MathScript industry;
	industry.loadMathScript(industryInput);
	V3::MathScript invalid;
	invalid.loadMathScript(invalidInput);

	V3::Country country;
	country.setSourceCountry(std::make_shared<EU4::Country>());

	EXPECT_FALSE(law.isValid(country));
	EXPECT_FALSE(tech.isValid(country));
	EXPECT_FALSE(colony.isValid(country));
	EXPECT_FALSE(tc.isValid(country));
	EXPECT_FALSE(gp.isValid(country));
	EXPECT_FALSE(industry.isValid(country));
	EXPECT_FALSE(invalid.isValid(country));


	V3::ProcessedData data;
	data.laws.emplace("law_serfdom");
	data.techs.emplace("centralization");
	data.industryFactor = 0.2;

	std::stringstream eu4Input;
	eu4Input << "colonial_parent = AAA";

	auto sourceCountry = std::make_shared<EU4::Country>("TAG", eu4Input);
	sourceCountry->setGP();
	sourceCountry->setTradeCompany();

	country.setProcessedData(data);
	country.setSourceCountry(sourceCountry);


	EXPECT_TRUE(law.isValid(country));
	EXPECT_TRUE(tech.isValid(country));
	EXPECT_TRUE(colony.isValid(country));
	EXPECT_TRUE(tc.isValid(country));
	EXPECT_TRUE(gp.isValid(country));
	EXPECT_TRUE(industry.isValid(country));
}

TEST(V3World_AddScriptTests, AddScriptsAdds)
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