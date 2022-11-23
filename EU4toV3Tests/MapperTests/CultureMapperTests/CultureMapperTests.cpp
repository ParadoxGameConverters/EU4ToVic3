#include "ClayManager/ClayManager.h"
#include "CultureLoader/CultureLoader.h"
#include "CultureMapper/CultureMapper.h"
#include "ReligionLoader/ReligionLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_CultureMapperTests, noMatchesGiveEmptyOptionalAndWarns)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	EXPECT_FALSE(culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "nonsense", "", "SOMESTATE", ""));
	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] ! CultureMapper - Attempting to match culture nonsense in state SOMESTATE failed.)"));
}

TEST(Mappers_CultureMapperTests, simpleMatchMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture1", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture1", "somereligion", "SOMESTATE", "TAG"));
}

TEST(Mappers_CultureMapperTests, simpleMatchMatchesBetweenMultipleCultures)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture2", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture3", "somereligion", "SOMESTATE", "TAG"));
}

TEST(Mappers_CultureMapperTests, religionOwnerMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture4", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture4", "religion_2", "SOMESTATE", "TAG"));
}

TEST(Mappers_CultureMapperTests, religionMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture5", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture4", "religion_2", "SOMESTATE", "GAT"));
}

TEST(Mappers_CultureMapperTests, cultureFallbackMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture6", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture4", "religion_3", "SOMESTATE", "GAT"));
}

TEST(Mappers_CultureMapperTests, superRegionMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture7", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture5", "somereligion", "STATE_TEST_4", "GAT"));
}

TEST(Mappers_CultureMapperTests, regionMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture8", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture5", "somereligion", "STATE_TEST_3", "GAT"));
}

TEST(Mappers_CultureMapperTests, stateMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture9", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture5", "somereligion", "STATE_TEST_1", "GAT"));
}

TEST(Mappers_CultureMapperTests, regionReligionMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture10", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture5", "religion_3", "STATE_TEST_2", "GAT"));
}

TEST(Mappers_CultureMapperTests, regionNonReligionMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture11", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture5", "religion_1", "STATE_TEST_2", "GAT"));
}

TEST(Mappers_CultureMapperTests, cultureGroupRegionReligionMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture12", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture6", "religion_2", "STATE_TEST_2", "GAT"));
}

TEST(Mappers_CultureMapperTests, cultureGroupReligionMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture13", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture6", "religion_2", "STATE_TEST_3", "GAT"));
}

TEST(Mappers_CultureMapperTests, cultureGroupRegionMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture14", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture6", "religion_1", "STATE_TEST_2", "GAT"));
}

TEST(Mappers_CultureMapperTests, cultureGroupMatches)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture15", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture6", "religion_1", "STATE_TEST_3", "GAT"));
}

TEST(Mappers_CultureMapperTests, macrosAreLoaded1)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture16", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture8", "religion_1", "STATE_TEST_2", "GAT"));
}

TEST(Mappers_CultureMapperTests, macrosAreLoaded2)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();
	auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "themod.mod"));
	commonItems::ModLoader modLoader;
	modLoader.loadMods("TestFiles", mods);
	mods = modLoader.getMods();
	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4Path, mods);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4Path, mods);

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");

	EXPECT_EQ("vculture17", *culMapper.cultureMatch(clayManager, cultureLoader, religionLoader, "culture8", "religion_1", "STATE_TEST_1", "GAT"));
}
