#include "ClayManager/ClayManager.h"
#include "CommonFunctions.h"
#include "CultureLoader/CultureLoader.h"
#include "CultureMapper/CultureMapper.h"
#include "LocalizationLoader/EU4LocalizationLoader.h"
#include "ReligionLoader/ReligionLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
const auto eu4FS = commonItems::ModFilesystem("TestFiles/eu4installation/", {Mod("Some mod", "TestFiles/mod/themod/")});

std::tuple<mappers::CultureMapper, V3::ClayManager, EU4::CultureLoader, EU4::ReligionLoader> prepMappers()
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4FS);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4FS);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");
	culMapper.loadWesternizationRules("TestFiles/configurables/westernization.txt");
	return std::tuple{culMapper, clayManager, cultureLoader, religionLoader};
}
} // namespace

TEST(Mappers_CultureMapperTests, noMatchesMatchesRequestRecordsAndWarnsOnce)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	const auto& match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "nonsense", "", "SOMESTATE", "");
	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] ! CultureMapper - Attempting to match culture nonsense in state SOMESTATE failed.)"));
	EXPECT_EQ("nonsense", *match);
	EXPECT_TRUE(culMapper.getUnMappedCultures().contains("nonsense"));
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
	EXPECT_EQ("culture6", *match); // this remains a dynamic name as there's no direct cultural group mapping for culture6.
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

	EXPECT_FALSE(culMapper.getUnMappedCultures().contains("unmapped_culture"));

	// now expand
	culMapper.expandCulturalMappings(clayManager, cultureLoader, religionLoader);

	EXPECT_TRUE(culMapper.getUnMappedCultures().contains("unmapped_culture"));

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

	culMapper.loadCultureDefinitions(modFS);
	culMapper.generateCultureDefinitions("TestFiles/configurables/name_lists.txt",
		 "TestFiles/configurables/name_list_map.txt",
		 "TestFiles/configurables/culture_trait_map.txt",
		 clayManager,
		 cultureLoader,
		 religionLoader,
		 eu4LocLoader);

	// eu4 cultures.txt defines cultures1-10 + unmapped_culture
	// culture_map.txt maps everything except culture5 and unmapped_culture
	// Those two will have defs generated as all the others are EXPECTED to have preset defs in vanilla + blankmod.
	// (in reality they don't but that's not relevant).
	//
	// In addition, inside are vanilla defs for vculture1 and vculture2 which are not interesting.

	ASSERT_EQ(10, culMapper.getV3CultureDefinitions().size());
	EXPECT_TRUE(culMapper.getV3CultureDefinitions().contains("vculture1"));
	EXPECT_TRUE(culMapper.getV3CultureDefinitions().contains("vculture2"));
	const auto& def1 = culMapper.getV3CultureDefinitions().at("culture5");
	const auto& def2 = culMapper.getV3CultureDefinitions().at("unmapped_culture");

	// culture 5 maps via:
	// link = { eu4group = culture_group trait = testtrait2 ethnicity = testable2 graphics = gr2 }
	// link = { eu4group = culture_group name_pool = name_list_test2 }
	EXPECT_EQ("culture5", def1.name);
	ASSERT_TRUE(def1.color);
	EXPECT_EQ("= rgb { 112 254 237 }", (*def1.color).outputRgb());
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
	EXPECT_EQ(10, culMapper.getWesternizationScoreForCulture("culture5"));
	EXPECT_EQ(5, culMapper.getLiteracyScoreForCulture("culture5"));
	EXPECT_EQ(5, culMapper.getIndustryScoreForCulture("culture5"));

	// unmapped_culture has no mapping links whatsoever and is scraping defaults
	EXPECT_EQ("unmapped_culture", def2.name);
	ASSERT_TRUE(def2.color);
	EXPECT_EQ("= rgb { 254 61 136 }", (*def2.color).outputRgb());
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
	EXPECT_EQ(0, culMapper.getWesternizationScoreForCulture("unmapped_culture"));
	EXPECT_EQ(0, culMapper.getLiteracyScoreForCulture("unmapped_culture"));
	EXPECT_EQ(0, culMapper.getIndustryScoreForCulture("unmapped_culture"));
}

TEST(Mappers_CultureMapperTests, cultureDefsLinksDynamicsToRelatedCultures)
{
	auto [culMapper, clayManager, cultureLoader, religionLoader] = prepMappers();
	culMapper.expandCulturalMappings(clayManager, cultureLoader, religionLoader);

	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " unmapped_culture: \"Unmapped\"\n";
	input << " culture5: \"Culture 5\"\n";
	EU4::EU4LocalizationLoader eu4LocLoader;
	eu4LocLoader.loadLocalizations(input);

	const auto& match = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "dynamic-culture1-culture7-culture-num1", "", "", "");
	ASSERT_TRUE(match);
	EXPECT_EQ("dynamic-culture1-culture7-culture-num1", *match); // this remains a dynamic culture.
	const auto& match2 = culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "", "", "");
	ASSERT_TRUE(match2);
	EXPECT_EQ("vculture1", *match2); // dynamic-culture1-culture7-culture-num1 should be related to vculture1.

	culMapper.loadCultureDefinitions(modFS);
	culMapper.generateCultureDefinitions("TestFiles/configurables/name_lists.txt",
		 "TestFiles/configurables/name_list_map.txt",
		 "TestFiles/configurables/culture_trait_map.txt",
		 clayManager,
		 cultureLoader,
		 religionLoader,
		 eu4LocLoader);

	const auto& related = culMapper.getRelatedCultures();

	ASSERT_TRUE(related.contains("vculture1"));
	EXPECT_THAT(related.at("vculture1"), testing::UnorderedElementsAre("dynamic-culture1-culture7-culture-num1"));
	ASSERT_TRUE(related.contains("vculture17"));
	EXPECT_THAT(related.at("vculture17"), testing::UnorderedElementsAre("dynamic-culture8-culture-num1"));
}

TEST(Mappers_CultureMapperTests, cultureMapperCanCompareCulturalTraits)
{
	mappers::CultureMapper culMapper;
	culMapper.loadCultureDefinitions(modFS);
	culMapper.loadTraitDefinitions(modFS);

	ASSERT_TRUE(culMapper.doCulturesShareHeritageTrait("vculture1", "vculture2"));
	EXPECT_FALSE(*culMapper.doCulturesShareHeritageTrait("vculture1", "vculture2"));
	ASSERT_TRUE(culMapper.doCulturesShareNonHeritageTrait("vculture1", "vculture2"));
	EXPECT_TRUE(*culMapper.doCulturesShareNonHeritageTrait("vculture1", "vculture2"));
}

TEST(Mappers_CultureMapperTests, cultureMapperDoesNotCompareBrokenCulturalTraits)
{
	mappers::CultureMapper culMapper;
	culMapper.loadCultureDefinitions(modFS);
	culMapper.loadTraitDefinitions(modFS);

	EXPECT_EQ(std::nullopt, culMapper.doCulturesShareHeritageTrait("vculture1", "nonsense"));
	EXPECT_EQ(std::nullopt, culMapper.doCulturesShareNonHeritageTrait("vculture1", "nonsense"));
}
