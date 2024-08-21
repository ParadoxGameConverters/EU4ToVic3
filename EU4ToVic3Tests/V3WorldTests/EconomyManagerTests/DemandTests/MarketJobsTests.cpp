#include "EconomyManager/Demand/MarketJobs.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
#include <ranges>


TEST(V3World_MarketJobsTests, DefaultsDefaultToDefaults)
{
	const V3::MarketJobs marketJobs;

	EXPECT_TRUE(marketJobs.getJobCounts().empty());
}

TEST(V3World_MarketJobsTests, JobBreakdownReturnsPercentages)
{
	std::map<std::string, double> jobList({{"poors", 250}, {"unemployed", 750}, {"peasants", 1000}});
	const V3::MarketJobs marketJobs(jobList, {});

	EXPECT_THAT(marketJobs.getJobBreakdown(),
		 testing::UnorderedElementsAre(testing::Pair("poors", .125), testing::Pair("unemployed", .375), testing::Pair("peasants", .5)));
}

TEST(V3World_MarketJobsTests, CreateJobsTakesFromUnemployedAndPeasants)
{
	std::map<std::string, double> jobList({{"poors", 250}, {"unemployed", 750}, {"peasants", 1000}});
	V3::MarketJobs marketJobs(jobList, {});

	std::stringstream input;
	input << "working_adult_ratio = 0.25\n";
	V3::PopType clerks;
	clerks.setType("clerks");

	marketJobs.createJobs(clerks, 250, 0.25, 0, 1);

	EXPECT_THAT(marketJobs.getJobCounts(),
		 testing::UnorderedElementsAre(testing::Pair("poors", 250),
			  testing::Pair("clerks", 1000),
			  testing::Pair("peasants", 750),
			  testing::Pair("unemployed", 0)));
}

// TODO(Gawquon): Test Manor House Removal