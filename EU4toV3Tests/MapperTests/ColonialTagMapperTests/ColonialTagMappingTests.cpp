#include "ClayManager/ClayManager.h"
#include "ClayManager/State/SubState.h"
#include "ColonialRegionMapper/ColonialRegionMapper.h"
#include "ColonialTagMapper/ColonialTagMapping.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ColonialTagMappingTests, MatchFailsForNoSubStates)
{

	std::stringstream input;
	input << "region = STATE_BRITISH_COLUMBIA tag = COL";
	const mappers::ColonialTagMapping mapping(input);

	const auto country = std::make_shared<V3::Country>();

	const auto& match = mapping.matchColonialTag(*country, {}, {});

	EXPECT_FALSE(match);
}

TEST(Mappers_ColonialTagMappingTests, MatchFailsForNoCapital)
{

	std::stringstream input;
	input << "region = STATE_BRITISH_COLUMBIA tag = COL";
	const mappers::ColonialTagMapping mapping(input);

	const auto country = std::make_shared<V3::Country>();
	const auto subState = std::make_shared<V3::SubState>();
	country->addSubState(subState);

	const auto& match = mapping.matchColonialTag(*country, {}, {});

	EXPECT_FALSE(match);
}

TEST(Mappers_ColonialTagMappingTests, MatchPassesForCorrectCapital)
{

	std::stringstream input;
	input << "region = STATE_BRITISH_COLUMBIA tag = COL";
	const mappers::ColonialTagMapping mapping(input);

	const auto country = std::make_shared<V3::Country>();
	const auto subState = std::make_shared<V3::SubState>();
	country->addSubState(subState);
	V3::ProcessedData data;
	data.capitalStateName = "STATE_BRITISH_COLUMBIA";
	country->setProcessedData(data);

	const auto& match = mapping.matchColonialTag(*country, {}, {});

	ASSERT_TRUE(match);
	EXPECT_EQ("COL", *match);
}
