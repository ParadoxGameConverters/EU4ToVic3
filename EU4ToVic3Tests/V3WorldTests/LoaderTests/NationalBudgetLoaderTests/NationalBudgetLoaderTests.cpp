#include "Loaders/NationalBudgetLoader/NationalBudgetLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>


TEST(V3World_NationalBudgetLoaderTests, NationalBudgetLoaderLoadsSectors)
{
	V3::NationalBudgetLoader nationalBudgetLoader;

	EXPECT_EQ(0, nationalBudgetLoader.getSectorBlueprints().size());
	nationalBudgetLoader.loadNationalBudget("TestFiles/configurables/economy/national_budget.txt");
	EXPECT_EQ(6, nationalBudgetLoader.getSectorBlueprints().size());

	std::vector<std::string> nameList = {"construction", "infrastructure", "test", "farming", "military", "extraction"};

	for (const auto& sectorBlueprint: nationalBudgetLoader.getSectorBlueprints())
	{
		EXPECT_THAT(std::vector{sectorBlueprint.getName()}, testing::IsSubsetOf(nameList));
	}
}

TEST(V3World_NationalBudgetLoaderTests, NationalBudgetLoaderBuildingsMapToSectors)
{
	V3::NationalBudgetLoader nationalBudgetLoader;
	nationalBudgetLoader.loadNationalBudget("TestFiles/configurables/economy/national_budget.txt");
	EXPECT_TRUE(nationalBudgetLoader.getBuildingSectorMap().empty());

	nationalBudgetLoader.buildBuildingSectorMap();

	EXPECT_THAT(nationalBudgetLoader.getBuildingSectorMap(),
		 testing::UnorderedElementsAre(std::pair("building_construction_center", "construction"),
			  std::pair("building_port", "infrastructure"),
			  std::pair("building_railway", "infrastructure"),
			  std::pair("building_barrack", "military"),
			  std::pair("building_fort", "military"),
			  std::pair("building_chemical_plant", "test"),
			  std::pair("building_steel_mill", "test"),
			  std::pair("building_motor_industry", "test"),
			  std::pair("building_munition_plant", "test"),
			  std::pair("building_rye_farm", "farming"),
			  std::pair("building_rice_farm", "farming"),
			  std::pair("building_wheat_farm", "farming"),
			  std::pair("building_maize_farm", "farming"),
			  std::pair("building_millet_farm", "farming"),
			  std::pair("building_livestock_ranch", "farming"),
			  std::pair("building_logging_camp", "extraction"),
			  std::pair("building_fishing_wharf", "extraction")));
}