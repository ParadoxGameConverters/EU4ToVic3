#include "CountryManager/EU4Country.h"
#include "LawMapper/LawMapper.h"
#include "Loaders/LawLoader/LawLoader.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(Mappers_LawMapperTests, LawsGroupsDefaultToEmpty)
{
	const mappers::LawMapper mapper;

	EXPECT_TRUE(mapper.getLawGroups().empty());
}

TEST(Mappers_LawMapperTests, LawsGroupsCanBeLoaded)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	const auto& lawGroups = mapper.getLawGroups();

	ASSERT_EQ(3, lawGroups.size());
	EXPECT_TRUE(lawGroups.contains("lawgroup_1"));
	EXPECT_EQ(1, lawGroups.at("lawgroup_1").size());
	EXPECT_TRUE(lawGroups.contains("lawgroup_2"));
	EXPECT_EQ(2, lawGroups.at("lawgroup_2").size());
	EXPECT_TRUE(lawGroups.contains("lawgroup_3"));
	EXPECT_EQ(1, lawGroups.at("lawgroup_3").size());
}

TEST(Mappers_LawMapperTests, MatchingDefinitionlessLawsThrowsWarningAndReturnsNullopt)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");

	// make a country, going for lawgroup_1's law_1 for reform1.
	const auto eu4Country = std::make_shared<EU4::Country>();
	eu4Country->addReform("reform1");
	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	const auto law = mapper.grantLawFromGroup("lawgroup_1", *country);
	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] We're trying to map to law: law_1 that has no definition! Skipping.)"));
	EXPECT_FALSE(law);
}

TEST(Mappers_LawMapperTests, SimpleLawsMatchesCanBeAssigned)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	V3::LawLoader loader;
	loader.loadLaws(modFS);
	mapper.loadLawDefinitions(loader.getLaws());

	// make a country, going for lawgroup_1's law_1 for reform1.
	const auto eu4Country = std::make_shared<EU4::Country>();
	eu4Country->addReform("reform1");
	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);

	const auto law = mapper.grantLawFromGroup("lawgroup_1", *country);
	ASSERT_TRUE(law);
	EXPECT_EQ("law_1", *law);
}

TEST(Mappers_LawMapperTests, HavingBlockingLawsPreventsAssignment)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	V3::LawLoader loader;
	loader.loadLaws(modFS);
	mapper.loadLawDefinitions(loader.getLaws());

	// make a country, going for lawgroup_3's law_4 for reform5.
	const auto eu4Country = std::make_shared<EU4::Country>();
	eu4Country->addReform("reform5");
	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);
	V3::ProcessedData data;
	data.laws.emplace("law_1"); // law_1 will block law_4
	country->setProcessedData(data);

	const auto law = mapper.grantLawFromGroup("lawgroup_3", *country);
	ASSERT_FALSE(law);
}

TEST(Mappers_LawMapperTests, NotHavingBlockingLawsDoesNotPreventsAssignment)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	V3::LawLoader loader;
	loader.loadLaws(modFS);
	mapper.loadLawDefinitions(loader.getLaws());

	// make a country, going for lawgroup_3's law_4 for reform5.
	const auto eu4Country = std::make_shared<EU4::Country>();
	eu4Country->addReform("reform5");
	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);
	V3::ProcessedData data;
	data.laws.emplace("law_2"); // law_2 will not block law_4
	country->setProcessedData(data);

	const auto law = mapper.grantLawFromGroup("lawgroup_3", *country);
	ASSERT_TRUE(law);
	EXPECT_EQ("law_4", *law);
}

TEST(Mappers_LawMapperTests, HavingMissingTechsPreventsAssignment)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	V3::LawLoader loader;
	loader.loadLaws(modFS);
	mapper.loadLawDefinitions(loader.getLaws());

	// make a country, going for lawgroup_2's law_2 for reforms 2 and 3 (reform 4 goes to law_3 but it's 1 score less).
	const auto eu4Country = std::make_shared<EU4::Country>();
	eu4Country->addReform("reform2");
	eu4Country->addReform("reform3");
	eu4Country->addReform("reform4");
	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country); // we have 0 techs, we won't get either law_2 or law_3.

	const auto law = mapper.grantLawFromGroup("lawgroup_2", *country);
	ASSERT_FALSE(law);
}

TEST(Mappers_LawMapperTests, HavingMissingTechsAllowsForNextBestAssignment)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	V3::LawLoader loader;
	loader.loadLaws(modFS);
	mapper.loadLawDefinitions(loader.getLaws());

	// make a country, going for lawgroup_2's law_2 for reforms 2 and 3 (reform 4 goes to law_3 but it's 1 score less).
	const auto eu4Country = std::make_shared<EU4::Country>();
	eu4Country->addReform("reform2");
	eu4Country->addReform("reform3");
	eu4Country->addReform("reform4");
	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);
	V3::ProcessedData data;
	data.techs.emplace("tech2"); // tech2 only allows for law_3, not law_2 we'd prefer.
	data.laws.emplace("law_1");  // law_3 also requires law_1.
	country->setProcessedData(data);

	const auto law = mapper.grantLawFromGroup("lawgroup_2", *country);
	ASSERT_TRUE(law);
	EXPECT_EQ("law_3", *law);
}

TEST(Mappers_LawMapperTests, HavingMissingLawsBlocksMatchAssignment)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	V3::LawLoader loader;
	loader.loadLaws(modFS);
	mapper.loadLawDefinitions(loader.getLaws());

	// make a country, going for lawgroup_2's law_2 for reforms 2 and 3 (reform 4 goes to law_3 but it's 1 score less).
	const auto eu4Country = std::make_shared<EU4::Country>();
	eu4Country->addReform("reform2");
	eu4Country->addReform("reform3");
	eu4Country->addReform("reform4");
	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);
	V3::ProcessedData data;
	data.techs.emplace("tech2"); // tech2 only allows for law_3, not law_2 we'd prefer, but we're MISSING law_1!
	country->setProcessedData(data);

	const auto law = mapper.grantLawFromGroup("lawgroup_2", *country);
	ASSERT_FALSE(law);
}

TEST(Mappers_LawMapperTests, HavingRequiredTechsAllowsForBestAssignment)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	V3::LawLoader loader;
	loader.loadLaws(modFS);
	mapper.loadLawDefinitions(loader.getLaws());

	// make a country, going for lawgroup_2's law_2 for reforms 2 and 3 (reform 4 goes to law_3 but it's 1 score less).
	const auto eu4Country = std::make_shared<EU4::Country>();
	eu4Country->addReform("reform2");
	eu4Country->addReform("reform3");
	eu4Country->addReform("reform4");
	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);
	V3::ProcessedData data;
	data.techs.emplace("tech1"); // tech1 allows for law_2 which we're prefering.
	data.techs.emplace("tech2"); // tech2 allows for law_3, not law_2 we'd prefer.
	data.laws.emplace("law_1");  // law_3 also requires law_1, otherwise it's not an option.
	country->setProcessedData(data);

	const auto law = mapper.grantLawFromGroup("lawgroup_2", *country);
	ASSERT_TRUE(law);
	EXPECT_EQ("law_2", *law);
}

TEST(Mappers_LawMapperTests, LawsFromSameGroupAsExistingLawsDoNotGetConsidered)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	V3::LawLoader loader;
	loader.loadLaws(modFS);
	mapper.loadLawDefinitions(loader.getLaws());

	// make a country, going for lawgroup_2's law_2 for reforms 2 and 3, however law_3 is already assigned!
	const auto eu4Country = std::make_shared<EU4::Country>();
	eu4Country->addReform("reform2");
	eu4Country->addReform("reform3");
	eu4Country->addReform("reform4");
	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);
	V3::ProcessedData data;
	data.techs.emplace("tech1");
	data.techs.emplace("tech2");
	data.laws.emplace("law_1");
	data.laws.emplace("law_3");
	country->setProcessedData(data);

	const auto law = mapper.grantLawFromGroup("lawgroup_2", *country);
	ASSERT_FALSE(law);
}

TEST(Mappers_LawMapperTests, NoMatchesDefaultsToBestNonBlockedLaw)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	V3::LawLoader loader;
	loader.loadLaws(modFS);
	mapper.loadLawDefinitions(loader.getLaws());

	// Zero reforms and law_1 means both law_2 and law_3 are viable. law_3 has higher progressiveness though.
	const auto eu4Country = std::make_shared<EU4::Country>();
	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);
	V3::ProcessedData data;
	data.techs.emplace("tech1");
	data.techs.emplace("tech2");
	data.laws.emplace("law_1");
	country->setProcessedData(data);

	const auto law = mapper.grantLawFromGroup("lawgroup_2", *country);
	ASSERT_TRUE(law);
	EXPECT_EQ("law_3", *law);
}