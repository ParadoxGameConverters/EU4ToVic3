#include "ClayManager/ClayManager.h"
#include "CultureLoader/CultureLoader.h"
#include "CultureMapper/CultureMapper.h"
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
