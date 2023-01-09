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
		EXPECT_THAT(std::vector<std::string>{sectorBlueprint.getName()}, testing::IsSubsetOf(nameList));
	}
}