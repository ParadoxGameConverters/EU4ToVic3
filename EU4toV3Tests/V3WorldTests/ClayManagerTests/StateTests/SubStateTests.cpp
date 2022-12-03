#include "ClayManager/ClayManager.h"
#include "ClayManager/State/Province.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
#include "PoliticalManager/Country/Country.h"
#include "ClayManager/ClayManager.h"
#include "Mappers/CultureMapper/CultureMapper.h"
#include "Mappers/ReligionMapper/ReligionMapper.h"
#include "CultureLoader/CultureLoader.h"
#include "ReligionLoader/ReligionLoader.h"

TEST(V3World_SubStateTests, OwnerTagCanBeSetAndRetrieved)
{
	V3::SubState subState;

	EXPECT_FALSE(subState.getOwnerTag());
	
	const auto owner = std::make_shared<V3::Country>();
	owner->setTag("AAA");
	subState.setOwner(owner);

	EXPECT_EQ("AAA", subState.getOwnerTag());
}

TEST(V3World_SubStateTests, HomeStateNameCanBeSetAndRetrieved)
{
	V3::SubState subState;

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	auto name = subState.getHomeStateName();
	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Attempted to access the name of a nullptr state from a substate. Returning empty name.)"));
	EXPECT_TRUE(name.empty());

	auto state = std::make_shared<V3::State>();
	state->setStateName("TEST_NAME");
	subState.setHomeState(state);

	name = subState.getHomeStateName();

	EXPECT_EQ("TEST_NAME", name);
}

TEST(V3World_SubStateTests, SubStateCanConvertDemograhics)
{
	V3::SubState subState;
	auto state = std::make_shared<V3::State>();
	state->setStateName("STATE_TEST_LAND1");
	subState.setHomeState(state);
	const auto owner = std::make_shared<V3::Country>();
	owner->setTag("AAA");
	subState.setOwner(owner);

	// let's make some source data.
	auto ratio = EU4::PopRatio("culture4", "anglican");
	V3::SourceProvinceData data;
	data.popRatios.push_back(ratio);
	subState.setSourceProvinceData({std::pair(data, 1)});

	// prep mappers.
	V3::ClayManager clayManager;
	mappers::ReligionMapper religionMapper;
	religionMapper.loadMappingRules("TestFiles/configurables/religion_map.txt");
	mappers::CultureMapper cultureMapper;
	cultureMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");
	EU4::CultureLoader cultureLoader;
	EU4::ReligionLoader religionLoader;

	subState.convertDemographics(clayManager, cultureMapper, religionMapper, cultureLoader, religionLoader);

	ASSERT_EQ(1, subState.getDemographics().size());

	auto demo = subState.getDemographics()[0];

	// link = { vic3 = vculture4 eu4 = culture4 religion = religion_2 owner = TAG }
	// link = {vic3 = vculture5 eu4 = culture4 religion = religion_2}
	// link = {vic3 = vculture6 eu4 = culture4}

	// link = { vic3 = protestant eu4 = protestant eu4 = reformed eu4 = anglican }

	EXPECT_EQ("vculture6", demo.culture);
	EXPECT_EQ("protestant", demo.religion);
	EXPECT_EQ(1, demo.upperRatio);
	EXPECT_EQ(1, demo.middleRatio);
	EXPECT_EQ(1, demo.lowerRatio);
}
