#include "ReligionLoader/ReligionLoader.h"
#include "ReligionLoader/ReligionParser.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
#include <ranges>
using testing::UnorderedElementsAre;

TEST(EU4World_ReligionLoaderTests, religionsDefaultToEmpty)
{
	std::stringstream input;
	EU4::ReligionLoader theReligions;
	theReligions.loadReligions(input);

	EXPECT_TRUE(theReligions.getAllReligions().empty());
}

TEST(EU4World_ReligionLoaderTests, religionNamesCanBeImported)
{
	const auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "TestFiles/mod/themod/"));

	EU4::ReligionLoader theReligions;
	theReligions.loadReligions(eu4Path, mods);

	std::set<std::string> religionNames;
	for (const auto& name: theReligions.getAllReligions() | std::views::keys)
		religionNames.emplace(name);

	EXPECT_THAT(religionNames, UnorderedElementsAre("religion_2", "religion_3", "converted_dynamic_faith_107", "converted_dynamic_faith_108"));
}

TEST(EU4World_ReligionLoaderTests, religionsImportWithCorrectReligiousGroups)
{
	const auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "TestFiles/mod/themod/"));

	EU4::ReligionLoader theReligions;
	theReligions.loadReligions(eu4Path, mods);

	EXPECT_THAT(theReligions.getAllReligions(),
		 UnorderedElementsAre(std::pair("religion_2", EU4::Religion("religion_2", "mod_group_1")),
			  std::pair("religion_3", EU4::Religion("religion_3", "mod_group_1")),
			  std::pair("converted_dynamic_faith_107", EU4::Religion("converted_dynamic_faith_107", "mod_group_2", "shamanism")),
			  std::pair("converted_dynamic_faith_108", EU4::Religion("converted_dynamic_faith_108", "mod_group_3"))));
}

TEST(EU4World_ReligionLoaderTests, trappingsAreLoadedForSaneCustomReligions)
{
	const auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "TestFiles/mod/themod/"));

	EU4::ReligionLoader theReligions;
	theReligions.loadReligions(eu4Path, mods);

	const auto& religion107 = theReligions.getAllReligions().at("converted_dynamic_faith_107");
	const auto& religion108 = theReligions.getAllReligions().at("converted_dynamic_faith_108");

	EXPECT_EQ("shamanism", religion107.trappings); // 99_converted_dynamic_faith_107-from-shamanism.txt
	EXPECT_TRUE(religion108.trappings.empty());	  // 99_converted_dynamic_faith_108-from-.txt
}

TEST(EU4World_ReligionLoaderTests, loadingInsaneCustomReligionsThrowsWarnings)
{
	const auto eu4Path = "TestFiles/eu4installation/";
	Mods mods;
	mods.emplace_back(Mod("Some mod", "TestFiles/mod/themod/"));

	EU4::ReligionLoader theReligions;

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	theReligions.loadReligions(eu4Path, mods);
	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] ! Religion Mapper: Cannot parse filename for metadata: 99_converted_dynamic_faith_108-from-.txt)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] ! Religion Mapper: Filename for custom religion is broken: 99_converted_dynamic_faith_error-.txt)"));
}

TEST(EU4World_ReligionLoaderTests, GroupForReligionCanBeRetrieved)
{
	std::stringstream input;
	input << "groupA = { religionA = {} }\n";
	input << "groupB = { religionB = {} }\n";
	EU4::ReligionLoader groups;
	groups.loadReligions(input);

	EXPECT_EQ("groupA", groups.getGroupForReligion("religionA"));
	EXPECT_EQ("groupB", groups.getGroupForReligion("religionB"));
}

TEST(EU4World_ReligionLoaderTests, GroupForReligionReturnsNulloptForFailure)
{
	std::stringstream input;
	input << "groupA = { religionA = {} }\n";
	input << "groupB = { religionB = {} }\n";
	EU4::ReligionLoader groups;
	groups.loadReligions(input);

	EXPECT_EQ(std::nullopt, groups.getGroupForReligion("religionC"));
}
