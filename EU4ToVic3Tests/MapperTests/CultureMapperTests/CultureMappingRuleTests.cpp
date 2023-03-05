#include "ClayManager/ClayManager.h"
#include "CultureLoader/CultureLoader.h"
#include "CultureMapper/CultureMappingRule.h"
#include "ReligionLoader/ReligionLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(Mappers_CultureMappingRuleTests, primitivesDefaultsToBlank)
{
	const mappers::CultureMappingRule mapping;

	EXPECT_TRUE(mapping.getRequestedMacros().empty());
	EXPECT_TRUE(mapping.getCultures().empty());
	EXPECT_TRUE(mapping.getCultureGroups().empty());
	EXPECT_TRUE(mapping.getReligions().empty());
	EXPECT_TRUE(mapping.getReligionGroups().empty());
	EXPECT_TRUE(mapping.getRegions().empty());
	EXPECT_TRUE(mapping.getReligionGroups().empty());
	EXPECT_TRUE(mapping.getV3Culture().empty());
}

TEST(Mappers_CultureMappingRuleTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "vic3 = vic3Culture eu4 = culture1 eu4 = culture2 eu4group = cgroup1 eu4group = cgroup2";
	input << " religion = religion1 religion = religion2 religion_group = rgroup1 religion_group = rgroup2";
	input << " @macro1 @macro2 @macro3 ";
	input << " region = region1 region = region2 owner = TAG1 owner = TAG2 }";

	mappers::CultureMappingRule mapping;
	mapping.loadMappingRules(input);

	EXPECT_EQ("vic3Culture", mapping.getV3Culture());
	EXPECT_THAT(mapping.getCultures(), testing::UnorderedElementsAre("culture1", "culture2"));
	EXPECT_THAT(mapping.getCultureGroups(), testing::UnorderedElementsAre("cgroup1", "cgroup2"));
	EXPECT_THAT(mapping.getReligions(), testing::UnorderedElementsAre("religion1", "religion2"));
	EXPECT_THAT(mapping.getReligionGroups(), testing::UnorderedElementsAre("rgroup1", "rgroup2"));
	EXPECT_THAT(mapping.getRegions(), testing::UnorderedElementsAre("region1", "region2"));
	EXPECT_THAT(mapping.getOwners(), testing::UnorderedElementsAre("TAG1", "TAG2"));
	EXPECT_THAT(mapping.getRequestedMacros(), testing::UnorderedElementsAre("@macro1", "@macro2", "@macro3"));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnSimpleCulture)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1");

	EXPECT_EQ("vculture1", *theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnSimpleCultureFailsForWrongCulture)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture2", "", "", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnSimpleCultureGroup)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;
	std::stringstream input;
	input << "cgroup1 = { culture1 = {}}";
	cultureLoader.loadCultures(input);

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4group = cgroup1");

	EXPECT_EQ("vculture1", *theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnSimpleCultureFailsForWrongCultureGroup)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;
	std::stringstream input;
	input << "cgroup1 = { culture1 = {}}";
	cultureLoader.loadCultures(input);

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4group = cgroup1");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture2", "", "", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnReligion)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	std::stringstream input;
	input << "rgroup1 = { religion1 = {}}";
	religionLoader.loadReligions(input);
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 religion = religion1");

	const auto match = theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "religion1", "", "");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture1", *match);
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnReligionFailsForNoReligion)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	std::stringstream input;
	input << "rgroup1 = { religion1 = {}}";
	religionLoader.loadReligions(input);
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 religion = religion1");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnReligionFailsForWrongReligion)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	std::stringstream input;
	input << "rgroup1 = { religion1 = {}}";
	religionLoader.loadReligions(input);
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 religion = religion1");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "religion2", "", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnReligionGroup)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	std::stringstream input;
	input << "rgroup1 = { religion1 = {}}";
	religionLoader.loadReligions(input);
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 religion_group = rgroup1");

	const auto match = theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "religion1", "", "");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture1", *match);
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnReligionFailsForNoReligionGroup)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	std::stringstream input;
	input << "rgroup1 = { religion1 = {}}";
	religionLoader.loadReligions(input);
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 religion_group = rgroup1");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnReligionFailsForWrongReligionGroup)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	std::stringstream input;
	input << "rgroup2 = { religion1 = {}}";
	religionLoader.loadReligions(input);
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 religion_group = rgroup1");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "religion1", "", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnOwner)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 owner = TAG");

	const auto match = theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "", "TAG");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture1", *match);
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnOwnerFailsForWrongOwner)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 owner = TAG");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "", "GAT"));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnOwnerFailsForNoOwner)
{
	V3::ClayManager clayManager;
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 owner = TAG");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnState)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 region = STATE_TEST_1");

	const auto match = theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "STATE_TEST_1", "");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture1", *match);
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnRegion)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 region = region_a");

	const auto match = theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "STATE_TEST_1", "");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture1", *match);
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnSuperRegion)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 region = test_1_strategic_regions");

	const auto match = theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "STATE_TEST_1", "");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture1", *match);
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnRegionFailsForWrongState)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 region = STATE_TEST_2");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "STATE_TEST_1", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnRegionFailsForWrongRegion)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 region = region_b");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "STATE_TEST_1", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnRegionFailsForWrongSuperRegion)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 region = test_2_strategic_regions");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "STATE_TEST_1", ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnRegionFailsForNoRegion)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 region = test_2_strategic_regions");

	EXPECT_FALSE(theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "", ""));
}

TEST(Mappers_CultureMappingRuleTests, regionalMatchOnRegion)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 region = test_1_strategic_regions");

	const auto match = theMapping.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "STATE_TEST_1", "");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture1", *match);
}

TEST(Mappers_CultureMappingRuleTests, regionalMatchOnFailsForNoLoadedRegions)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1");

	EXPECT_FALSE(theMapping.cultureRegionalMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "STATE_TEST_1", ""));
}

TEST(Mappers_CultureMappingRuleTests, nonRegionalNonReligionsMatch)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	std::stringstream input;
	input << "rgroup2 = { religion1 = {}}";
	religionLoader.loadReligions(input);
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1");

	const auto match = theMapping.cultureNonRegionalNonReligiousMatch(clayManager, cultureLoader, religionLoader, "culture1", "religion1", "STATE_TEST_1", "");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture1", *match);
}

TEST(Mappers_CultureMappingRuleTests, nonRegionalNonReligionsMatchOnFailsForLoadedRegions)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	std::stringstream input;
	input << "rgroup2 = { religion1 = {}}";
	religionLoader.loadReligions(input);
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 region = STATE_TEST_1");

	EXPECT_FALSE(theMapping.cultureNonRegionalNonReligiousMatch(clayManager, cultureLoader, religionLoader, "culture1", "religion1", "STATE_TEST_1", ""));
}

TEST(Mappers_CultureMappingRuleTests, nonRegionalNonReligionsMatchOnFailsForLoadedReligions)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	std::stringstream input;
	input << "rgroup2 = { religion1 = {}}";
	religionLoader.loadReligions(input);
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 religion = religion1");

	EXPECT_FALSE(theMapping.cultureNonRegionalNonReligiousMatch(clayManager, cultureLoader, religionLoader, "culture1", "religion1", "STATE_TEST_1", ""));
}

TEST(Mappers_CultureMappingRuleTests, nonRegionalNonReligionsMatchOnFailsForLoadedReligionGroups)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	std::stringstream input;
	input << "rgroup2 = { religion1 = {}}";
	religionLoader.loadReligions(input);
	EU4::CultureLoader cultureLoader;

	mappers::CultureMappingRule theMapping;
	theMapping.loadMappingRules("vic3 = vculture1 eu4 = culture1 religion_group = rgroup2");

	EXPECT_FALSE(theMapping.cultureNonRegionalNonReligiousMatch(clayManager, cultureLoader, religionLoader, "culture1", "religion1", "STATE_TEST_1", ""));
}
