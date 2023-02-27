#include "ClayManager/ClayManager.h"
#include "ClayManager/State/SubState.h"
#include "ColonialRegionMapper/ColonialRegionMapper.h"
#include "ColonialTagMapper/ColonialTagMapper.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"

TEST(Mappers_ColonialTagMapperTests, rulesCanBeLoadedAndMatched)
{
	mappers::ColonialTagMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/colonial_tags.txt");

	const auto country = std::make_shared<V3::Country>();
	const auto subState = std::make_shared<V3::SubState>();
	country->addSubState(subState);
	V3::ProcessedData data;
	data.capitalStateName = "STATE_BRITISH_COLUMBIA";
	country->setProcessedData(data);

	const auto& match = mapper.matchColonialTag(*country, {}, {});

	ASSERT_TRUE(match);
	EXPECT_EQ("COL", *match);
}

TEST(Mappers_ColonialTagMapperTests, MatchWillFailForExistingColonialTag)
{
	mappers::ColonialTagMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/colonial_tags.txt");

	const auto country = std::make_shared<V3::Country>();
	country->setTag("COL");
	const auto subState = std::make_shared<V3::SubState>();
	country->addSubState(subState);
	V3::ProcessedData data;
	data.capitalStateName = "STATE_BRITISH_COLUMBIA";
	country->setProcessedData(data);

	const auto& match = mapper.matchColonialTag(*country, {}, {});

	EXPECT_FALSE(match);
}

TEST(Mappers_ColonialTagMapperTests, MatchWillFailForNoMatches)
{
	mappers::ColonialTagMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/colonial_tags.txt");

	const auto country = std::make_shared<V3::Country>();
	const auto subState = std::make_shared<V3::SubState>();
	country->addSubState(subState);
	V3::ProcessedData data;
	data.capitalStateName = "STATE_WRONG_COLUMBIA";
	country->setProcessedData(data);

	const auto& match = mapper.matchColonialTag(*country, {}, {});

	EXPECT_FALSE(match);
}
