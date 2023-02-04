#include "Loaders/CountryDefinitionLoader/CountryDefinitionLoader.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_CountryDefinitionLoaderTests, CountryDefinitionLoaderCanLoadCountries)
{
	V3::CountryDefinitionLoader countryLoader;
	EXPECT_EQ(0, countryLoader.getCountries().size());
	countryLoader.loadCommonCountries(modFS);
	ASSERT_EQ(4, countryLoader.getCountries().size());

	const auto& country1 = countryLoader.getCountries().at("TAG");
	const auto& country2 = countryLoader.getCountries().at("GAT");
	const auto& country3 = countryLoader.getCountries().at("TGA");
	const auto& country4 = countryLoader.getCountries().at("CCC");

	EXPECT_EQ("TAG", country1->getTag());
	EXPECT_EQ("GAT", country2->getTag());
	EXPECT_EQ("TGA", country3->getTag());
	EXPECT_EQ("CCC", country4->getTag());
}

TEST(V3World_CountryDefinitionLoaderTests, nonTXTfilesAreIgnored)
{
	V3::CountryDefinitionLoader countryLoader;
	EXPECT_EQ(0, countryLoader.getCountries().size());
	countryLoader.loadCommonCountries(modFS);

	EXPECT_FALSE(countryLoader.getCountries().contains("IGN"));
}
