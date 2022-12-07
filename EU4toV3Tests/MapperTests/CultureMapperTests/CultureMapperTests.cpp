#include "ClayManager/ClayManager.h"
#include "CommonFunctions.h"
#include "CultureLoader/CultureLoader.h"
#include "CultureMapper/CultureMapper.h"
#include "LocalizationLoader/EU4LocalizationLoader.h"
#include "ReligionLoader/ReligionLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});

std::tuple<mappers::CultureMapper, V3::ClayManager, EU4::CultureLoader, EU4::ReligionLoader> prepMappers()
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "TestFiles/mod/themod/"));
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");
	return std::tuple{culMapper, clayManager, cultureLoader, religionLoader};
}

TEST(Mappers_CultureMapperTests, noMatchesGiveEmptyOptionalAndWarns)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	EXPECT_FALSE(culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "nonsense", "", "SOMESTATE", ""));
	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] ! CultureMapper - Attempting to match culture nonsense in state SOMESTATE failed.)"));
}

TEST(Mappers_CultureMapperTests, simpleMatchMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "somereligion", "SOMESTATE", "TAG");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture1", *match);
}

TEST(Mappers_CultureMapperTests, simpleMatchMatchesBetweenMultipleCultures)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture3", "somereligion", "SOMESTATE", "TAG");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture2", *match);
}

TEST(Mappers_CultureMapperTests, religionOwnerMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture4", "religion_2", "SOMESTATE", "TAG");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture4", *match);
}

TEST(Mappers_CultureMapperTests, religionMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture4", "religion_2", "SOMESTATE", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture5", *match);
}

TEST(Mappers_CultureMapperTests, cultureFallbackMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture4", "religion_3", "SOMESTATE", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture6", *match);
}

TEST(Mappers_CultureMapperTests, superRegionMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture5", "somereligion", "STATE_TEST_4", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture7", *match);
}

TEST(Mappers_CultureMapperTests, regionMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture5", "somereligion", "STATE_TEST_3", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture8", *match);
}

TEST(Mappers_CultureMapperTests, stateMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture5", "somereligion", "STATE_TEST_1", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture9", *match);
}

TEST(Mappers_CultureMapperTests, regionReligionMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture5", "religion_3", "STATE_TEST_2", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture10", *match);
}

TEST(Mappers_CultureMapperTests, regionNonReligionMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture5", "religion_1", "STATE_TEST_2", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture11", *match);
}

TEST(Mappers_CultureMapperTests, cultureGroupRegionReligionMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture6", "religion_2", "STATE_TEST_2", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture12", *match);
}

TEST(Mappers_CultureMapperTests, cultureGroupReligionMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture6", "religion_2", "STATE_TEST_3", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture13", *match);
}

TEST(Mappers_CultureMapperTests, cultureGroupRegionMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture6", "religion_1", "STATE_TEST_2", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture14", *match);
}

TEST(Mappers_CultureMapperTests, cultureGroupMatches)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture6", "religion_1", "STATE_TEST_3", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture15", *match);
}

TEST(Mappers_CultureMapperTests, macrosAreLoaded1)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture8", "religion_1", "STATE_TEST_2", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture16", *match);
}

TEST(Mappers_CultureMapperTests, macrosAreLoaded2)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture8", "religion_1", "STATE_TEST_1", "GAT");
	ASSERT_TRUE(match);
	EXPECT_EQ("vculture17", *match);
}

TEST(Mappers_CultureMapperTests, rulesCanBeExpandedWithUnmappedCultures)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	EXPECT_FALSE(culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "unmapped_culture", "", "SOMESTATE", ""));
	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] ! CultureMapper - Attempting to match culture unmapped_culture in state SOMESTATE failed.)"));

	// now expand
	culMapper.expandCulturalMappings(clayManager, cultureLoader, religionLoader);

	auto match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "unmapped_culture", "", "SOMESTATE", "");
	ASSERT_TRUE(match);
	EXPECT_EQ("unmapped_culture", *match);
}

TEST(Mappers_CultureMapperTests, cultureDefsCanBeGenerated)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();
	culMapper.expandCulturalMappings(clayManager, cultureLoader, religionLoader);

	EXPECT_TRUE(culMapper.getV3CultureDefinitions().empty());

	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " unmapped_culture: \"Unmapped\"\n";
	input << " culture5: \"Culture 5\"\n";
	EU4::EU4LocalizationLoader eu4LocLoader;
	eu4LocLoader.loadLocalizations(input);

	culMapper.generateCultureDefinitions(modFS,
		 "TestFiles/configurables/name_lists.txt",
		 "TestFiles/configurables/name_list_map.txt",
		 "TestFiles/configurables/culture_trait_map.txt",
		 cultureLoader,
		 eu4LocLoader);

	// eu4 cultures.txt defines cultures1-10 + unmapped_culture
	// culture_map.txt maps everything except culture5 and unmapped_culture
	// Those two will have defs generated as all the others are EXPECTED to have preset defs in vanilla + blankmod.
	// (in reality they don't but that's not relevant).
	//
	// In addition, inside are vanilla defs for vculture1 and vculture2 which are not interesting.

	ASSERT_EQ(4, culMapper.getV3CultureDefinitions().size());
	EXPECT_TRUE(culMapper.getV3CultureDefinitions().contains("vculture1"));
	EXPECT_TRUE(culMapper.getV3CultureDefinitions().contains("vculture2"));
	const auto& def1 = culMapper.getV3CultureDefinitions().at("culture5");
	const auto& def2 = culMapper.getV3CultureDefinitions().at("unmapped_culture");

	// culture 5 maps via:
	// link = { eu4group = culture_group trait = testtrait2 ethnicity = testable2 graphics = gr2 }
	// link = { eu4group = culture_group name_pool = name_list_test2 }
	EXPECT_EQ("culture5", def1.name);
	EXPECT_FALSE(def1.color);
	EXPECT_THAT(def1.traits, testing::UnorderedElementsAre("testtrait2"));
	EXPECT_THAT(def1.maleCommonFirstNames, testing::UnorderedElementsAre("male3", "male4"));
	EXPECT_THAT(def1.femaleCommonFirstNames, testing::UnorderedElementsAre("female3", "female4"));
	EXPECT_THAT(def1.nobleLastNames, testing::UnorderedElementsAre("dyn3", "dyn4"));
	EXPECT_THAT(def1.commonLastNames, testing::UnorderedElementsAre("dyn3", "dyn4"));
	EXPECT_THAT(def1.maleRegalFirstNames, testing::UnorderedElementsAre("male3", "male4"));
	EXPECT_THAT(def1.femaleRegalFirstNames, testing::UnorderedElementsAre("female3", "female4"));
	EXPECT_TRUE(def1.regalLastNames.empty()); // we don't map these atm.
	EXPECT_THAT(def1.ethnicities, testing::UnorderedElementsAre("testable2"));
	EXPECT_EQ("gr2", def1.graphics);
	EXPECT_EQ("Culture 5", def1.locBlock.at("english"));

	// unmapped_culture has no mapping links whatsoever and is scraping defaults
	EXPECT_EQ("unmapped_culture", def2.name);
	EXPECT_FALSE(def2.color);
	EXPECT_TRUE(def2.traits.empty());
	EXPECT_TRUE(def2.maleCommonFirstNames.empty());
	EXPECT_TRUE(def2.femaleCommonFirstNames.empty());
	EXPECT_TRUE(def2.nobleLastNames.empty());
	EXPECT_TRUE(def2.commonLastNames.empty());
	EXPECT_TRUE(def2.maleRegalFirstNames.empty());
	EXPECT_TRUE(def2.femaleRegalFirstNames.empty());
	EXPECT_TRUE(def2.regalLastNames.empty());											 // we don't map these atm.
	EXPECT_THAT(def2.ethnicities, testing::UnorderedElementsAre("neutral")); // fallback default
	EXPECT_EQ("generic", def2.graphics);												 // generic fallback for everyone.
	EXPECT_EQ("Unmapped", def2.locBlock.at("english"));
}
