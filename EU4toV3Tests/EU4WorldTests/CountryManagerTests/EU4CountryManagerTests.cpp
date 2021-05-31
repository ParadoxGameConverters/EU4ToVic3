#include "CountryManager/EU4CountryManager.h"
#include "gtest/gtest.h"

TEST(EU4World_CountryManagerTests, primitivesDefaultToBlank)
{
	const EU4::CountryManager manager;

	EXPECT_TRUE(manager.getCountries().empty());
}

TEST(EU4World_CountryManagerTests, countriesCanBeLoaded)
{
	std::stringstream input;
	input << "AAA = {}\n";
	input << "BBB = {}\n";
	EU4::CountryManager manager;
	manager.loadCountries(input);

	EXPECT_EQ(2, manager.getCountries().size());
	EXPECT_TRUE(manager.getCountries().contains("AAA"));
	EXPECT_TRUE(manager.getCountries().contains("BBB"));
}

TEST(EU4World_CountryManagerTests, SpecialCountriesAreIgnored)
{
	std::stringstream input;
	input << "--- = {}\n";
	input << "REB = {}\n";
	input << "PIR = {}\n";
	input << "NAT = {}\n";
	input << "AAA = {}\n";
	input << "BBB = {}\n";
	EU4::CountryManager manager;
	manager.loadCountries(input);

	EXPECT_EQ(2, manager.getCountries().size());
	EXPECT_TRUE(manager.getCountries().contains("AAA"));
	EXPECT_TRUE(manager.getCountries().contains("BBB"));
}

TEST(EU4World_CountryManagerTests, CountriesCanBeRetrieved)
{
	std::stringstream input;
	input << "AAA = {}\n";
	input << "BBB = {}\n";
	EU4::CountryManager manager;
	manager.loadCountries(input);

	EXPECT_EQ("AAA", manager.getCountry("AAA")->getTag());
	EXPECT_EQ("BBB", manager.getCountry("BBB")->getTag());
}

TEST(EU4World_CountryManagerTests, CountryMismatchReturnsNullptr)
{
	std::stringstream input;
	EU4::CountryManager manager;
	manager.loadCountries(input);

	EXPECT_EQ(nullptr, manager.getCountry("AAA"));
}
