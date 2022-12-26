#include "TechSetupMapper/TechSetupMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_TechSetupMapperTests, NonsenseTrackReturnEmptyTechsAndWarns)
{
	mappers::TechSetupMapper mapper;
	std::stringstream input;
	mapper.loadMappingRules(input);

	double score = 78.89;
	std::string track = "track";

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	const auto techs = mapper.getTechsForScoreTrack(track, score);
	std::cout.rdbuf(cout_buffer);

	EXPECT_TRUE(techs.empty());
	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Attempting to get techs from track: track which doesn't exist in tech_setup.txt!)"));
}

TEST(Mappers_TechSetupMapperTests, TechsCanBeRetrieved)
{
	mappers::TechSetupMapper mapper;
	std::stringstream input;
	input << "track = { tech1 = 10 tech2 = 20 tech3 = 30 tech4 = 40 tech5 = 50 }\n";
	input << "another_track = { atech1 = 10 atech2 = 20 atech3 = 30 atech4 = 40 atech5 = 50 }\n";
	mapper.loadMappingRules(input);

	EXPECT_THAT(mapper.getTechsForScoreTrack("track", 0), testing::UnorderedElementsAre());
	EXPECT_THAT(mapper.getTechsForScoreTrack("track", 10), testing::UnorderedElementsAre("tech1"));
	EXPECT_THAT(mapper.getTechsForScoreTrack("track", 15), testing::UnorderedElementsAre("tech1"));
	EXPECT_THAT(mapper.getTechsForScoreTrack("track", 25), testing::UnorderedElementsAre("tech1", "tech2"));
	EXPECT_THAT(mapper.getTechsForScoreTrack("another_track", 50), testing::UnorderedElementsAre("atech1", "atech2", "atech3", "atech4", "atech5"));
}
