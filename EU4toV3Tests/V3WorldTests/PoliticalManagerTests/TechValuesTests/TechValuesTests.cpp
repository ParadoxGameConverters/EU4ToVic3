#include "ClayManager/State/SubState.h"
#include "CountryManager/EU4Country.h"
#include "CountryManager/EU4CountryManager.h"
#include "PoliticalManager/Country/Country.h"
#include "PoliticalManager/TechValues/TechValues.h"
#include "gtest/gtest.h"

TEST(V3World_TechValuesTests, CountryIsValidFailsForLandlessCountries)
{
	V3::Country country;
	const auto eu4Country = std::make_shared<EU4::Country>();
	country.setSourceCountry(eu4Country);

	EXPECT_FALSE(V3::TechValues::isValidCountryForTechConversion(country));

	const auto subState = std::make_shared<V3::SubState>();
	country.addSubState(subState);

	EXPECT_TRUE(V3::TechValues::isValidCountryForTechConversion(country));
}

TEST(V3World_TechValuesTests, CountryIsValidFailsForSourceLessCountries)
{
	V3::Country country;
	const auto subState = std::make_shared<V3::SubState>();
	country.addSubState(subState);

	EXPECT_FALSE(V3::TechValues::isValidCountryForTechConversion(country));

	const auto eu4Country = std::make_shared<EU4::Country>();
	country.setSourceCountry(eu4Country);

	EXPECT_TRUE(V3::TechValues::isValidCountryForTechConversion(country));
}

TEST(V3World_TechValuesTests, CountryIsValidPassesForLandedEU4ImportedCountries)
{
	V3::Country country;
	const auto subState = std::make_shared<V3::SubState>();
	country.addSubState(subState);
	const auto eu4Country = std::make_shared<EU4::Country>();
	country.setSourceCountry(eu4Country);

	EXPECT_TRUE(V3::TechValues::isValidCountryForTechConversion(country));
}

TEST(V3World_TechValuesTests, TechValuesCanBeGeneratedAndPinged)
{
	// load base eu4 countries with some tech
	std::stringstream eu4input;
	eu4input << "TA1 = { technology = { adm_tech = 1 dip_tech = 5 mil_tech = 7 } }\n";
	eu4input << "TA2 = { technology = { adm_tech = 5 dip_tech = 1 mil_tech = 5 } }\n";
	eu4input << "TA3 = { technology = { adm_tech = 7 dip_tech = 7 mil_tech = 1 } }\n";
	EU4::CountryManager cm;
	cm.loadCountries(eu4input);

	// assign them to some vic3 countries (which must be landed, doesn't matter if land is shared)
	V3::ProcessedData data;
	data.civLevel = 100;
	auto GA1 = std::make_shared<V3::Country>();
	const auto subState = std::make_shared<V3::SubState>();
	GA1->addSubState(subState);
	GA1->setSourceCountry(cm.getCountry("TA1"));
	GA1->setProcessedData(data);
	auto GA2 = std::make_shared<V3::Country>();
	GA2->addSubState(subState);
	GA2->setSourceCountry(cm.getCountry("TA2"));
	GA2->setProcessedData(data);
	auto GA3 = std::make_shared<V3::Country>();
	GA3->addSubState(subState);
	GA3->setSourceCountry(cm.getCountry("TA3"));
	GA3->setProcessedData(data);
	const std::map<std::string, std::shared_ptr<V3::Country>> countries = {{"GA1", GA1}, {"GA2", GA2}, {"GA3", GA3}};

	// Sort them.
	V3::TechValues techValues(countries);

	// and ping the ratings
	EXPECT_DOUBLE_EQ(100.0 / 3.0, techValues.getSocietyTechPercentile("GA1"));
	EXPECT_DOUBLE_EQ(200.0 / 3.0, techValues.getProductionTechPercentile("GA1"));
	EXPECT_DOUBLE_EQ(300.0 / 3.0, techValues.getMilitaryTechPercentile("GA1"));

	EXPECT_DOUBLE_EQ(200.0 / 3.0, techValues.getSocietyTechPercentile("GA2"));
	EXPECT_DOUBLE_EQ(100.0 / 3.0, techValues.getProductionTechPercentile("GA2"));
	EXPECT_DOUBLE_EQ(200.0 / 3.0, techValues.getMilitaryTechPercentile("GA2"));

	EXPECT_DOUBLE_EQ(300.0 / 3.0, techValues.getSocietyTechPercentile("GA3"));
	EXPECT_DOUBLE_EQ(300.0 / 3.0, techValues.getProductionTechPercentile("GA3"));
	EXPECT_DOUBLE_EQ(100.0 / 3.0, techValues.getMilitaryTechPercentile("GA3"));
}

TEST(V3World_TechValuesTests, TechValuesAreCappedByCivLevel)
{
	// load base eu4 countries with some tech
	std::stringstream eu4input;
	eu4input << "TA1 = { technology = { adm_tech = 7 dip_tech = 7 mil_tech = 7 } }\n";
	eu4input << "TA2 = { technology = { adm_tech = 7 dip_tech = 7 mil_tech = 7 } }\n";
	eu4input << "TA3 = { technology = { adm_tech = 7 dip_tech = 7 mil_tech = 7 } }\n";
	EU4::CountryManager cm;
	cm.loadCountries(eu4input);

	// assign them to some vic3 countries (which must be landed, doesn't matter if land is shared)
	V3::ProcessedData data;
	auto GA1 = std::make_shared<V3::Country>();
	const auto subState = std::make_shared<V3::SubState>();
	GA1->addSubState(subState);
	GA1->setSourceCountry(cm.getCountry("TA1"));
	data.civLevel = 100; // first
	GA1->setProcessedData(data);
	auto GA2 = std::make_shared<V3::Country>();
	GA2->addSubState(subState);
	GA2->setSourceCountry(cm.getCountry("TA2"));
	data.civLevel = 10; // third
	GA2->setProcessedData(data);
	auto GA3 = std::make_shared<V3::Country>();
	GA3->addSubState(subState);
	GA3->setSourceCountry(cm.getCountry("TA3"));
	data.civLevel = 50; // second
	GA3->setProcessedData(data);
	const std::map<std::string, std::shared_ptr<V3::Country>> countries = {{"GA1", GA1}, {"GA2", GA2}, {"GA3", GA3}};

	// Sort them.
	V3::TechValues techValues(countries);

	// and ping the ratings
	EXPECT_DOUBLE_EQ(300.0 / 3.0, techValues.getSocietyTechPercentile("GA1"));
	EXPECT_DOUBLE_EQ(300.0 / 3.0, techValues.getProductionTechPercentile("GA1"));
	EXPECT_DOUBLE_EQ(300.0 / 3.0, techValues.getMilitaryTechPercentile("GA1"));

	EXPECT_DOUBLE_EQ(100.0 / 3.0, techValues.getSocietyTechPercentile("GA2"));
	EXPECT_DOUBLE_EQ(100.0 / 3.0, techValues.getProductionTechPercentile("GA2"));
	EXPECT_DOUBLE_EQ(100.0 / 3.0, techValues.getMilitaryTechPercentile("GA2"));

	EXPECT_DOUBLE_EQ(200.0 / 3.0, techValues.getSocietyTechPercentile("GA3"));
	EXPECT_DOUBLE_EQ(200.0 / 3.0, techValues.getProductionTechPercentile("GA3"));
	EXPECT_DOUBLE_EQ(200.0 / 3.0, techValues.getMilitaryTechPercentile("GA3"));
}
