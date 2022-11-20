#include "PoliticalManager/PoliticalManager.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_PoliticalManagerTests, PoliticalManagerCanInitializeVanillaCountries)
{
	V3::PoliticalManager politicalManager;

	EXPECT_EQ(0, politicalManager.getCountries().size());

	politicalManager.initializeVanillaCountries("TestFiles/vic3installation/game/");

	EXPECT_EQ(3, politicalManager.getCountries().size());

	const auto& country1 = politicalManager.getCountries().at("TAG");
	const auto& country2 = politicalManager.getCountries().at("GAT");
	const auto& country3 = politicalManager.getCountries().at("TGA");

	EXPECT_EQ("TAG", country1->getTag());
	EXPECT_EQ("GAT", country2->getTag());
	EXPECT_EQ("TGA", country3->getTag());
}
