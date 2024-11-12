#include "ClayManager/State/SubState.h"
#include "EconomyManager/Demand/MarketJobs.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
#include <ranges>

namespace
{
std::map<std::string, V3::PopType> getPopTypes()
{
	std::map<std::string, V3::PopType> popTypes;

	std::stringstream input;
	input << "working_adult_ratio = 0.2\n";

	V3::PopType aristocrats(input), laborers, clerks, capitalists, peasants, farmers;

	popTypes.emplace("aristocrats", aristocrats);
	popTypes.emplace("laborers", laborers);
	popTypes.emplace("clerks", clerks);
	popTypes.emplace("capitalists", capitalists);
	popTypes.emplace("peasants", peasants);
	popTypes.emplace("farmers", farmers);

	return popTypes;
}
} // namespace

TEST(V3World_MarketJobsTests, CreateSubsistenceLeavesUnemployedPopsWhenNotEnoughLand)
{
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 5000});

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});
	double levels = marketJobs.createSubsistence({{"peasants", 90}, {"farmers", 10}}, .25, 0, 5, getPopTypes(), subState);

	const double delta = 0.0000001;
	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(250, delta)),
			  testing::Pair("laborers", testing::DoubleNear(1800, delta)),
			  testing::Pair("peasants", testing::DoubleNear(1800, delta)),
			  testing::Pair("farmers", testing::DoubleNear(200, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(950, delta))));
	EXPECT_DOUBLE_EQ(5, levels);
}

TEST(V3World_MarketJobsTests, CreateSubsistenceLeavesMoreUnemployedPopsWhenMoreWomenWork)
{
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 5000});

	constexpr double moreWomenWorking = 0.05;

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});
	double levels = marketJobs.createSubsistence({{"peasants", 90}, {"farmers", 10}}, .25, moreWomenWorking, 5, getPopTypes(), subState);

	const double delta = 0.001;
	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(200, delta)),
			  testing::Pair("laborers", testing::DoubleNear(1500, delta)),
			  testing::Pair("peasants", testing::DoubleNear(1500, delta)),
			  testing::Pair("farmers", testing::DoubleNear(166.666, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(1633.333, delta))));
	EXPECT_DOUBLE_EQ(5, levels);
}

TEST(V3World_MarketJobsTests, CreateSubsistenceLeavesNoUnemployedPopsWhenEnoughLand)
{
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 5000});

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});
	double levels = marketJobs.createSubsistence({{"peasants", 90}, {"farmers", 10}}, .25, 0, 20, getPopTypes(), subState);

	const double delta = 0.001;
	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(308.642, delta)),
			  testing::Pair("laborers", testing::DoubleNear(2222.222, delta)),
			  testing::Pair("peasants", testing::DoubleNear(2222.222, delta)),
			  testing::Pair("farmers", testing::DoubleNear(246.914, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(0, delta))));
	EXPECT_DOUBLE_EQ(6.17283950617284, levels);
}

TEST(V3World_MarketJobsTests, CreateSubsistenceUsesHomesteadingLawsToPickNumbersAndTypesOfPopsWorkingSubsistence)
{
	// Just peasants, no farmers
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 5000});

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});
	double levels = marketJobs.createSubsistence({{"peasants", 100}}, .25, 0, 5, getPopTypes(), subState);

	const double delta = 0.0000001;
	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(250, delta)),
			  testing::Pair("laborers", testing::DoubleNear(1800, delta)),
			  testing::Pair("peasants", testing::DoubleNear(2000, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(950, delta))));
	EXPECT_DOUBLE_EQ(5, levels);
}

TEST(V3World_MarketJobsTests, CreateJobsTakesFromUnemployedPopsFirst)
{
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 10000});

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});
	std::map<std::string, double> subsistenceEmployment{{"peasants", 90}, {"farmers", 10}};
	marketJobs.createSubsistence(subsistenceEmployment, .25, 0, 5, getPopTypes(), subState);

	const double delta = 0.0000001;
	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(250, delta)),
			  testing::Pair("laborers", testing::DoubleNear(1800, delta)),
			  testing::Pair("peasants", testing::DoubleNear(1800, delta)),
			  testing::Pair("farmers", testing::DoubleNear(200, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(5950, delta))));

	const double levels = marketJobs.createJobs({{"laborers", 100}, {"clerks", 100}}, subsistenceEmployment, 5, .25, 0, {}, {}, getPopTypes(), subState);

	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(250, delta)),
			  testing::Pair("laborers", testing::DoubleNear(3800, delta)),
			  testing::Pair("peasants", testing::DoubleNear(1800, delta)),
			  testing::Pair("clerks", testing::DoubleNear(2000, delta)),
			  testing::Pair("farmers", testing::DoubleNear(200, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(1950, delta))));
	EXPECT_DOUBLE_EQ(0, levels);
}

TEST(V3World_MarketJobsTests, CreateJobsTakesFromPeasantPopsAfterUnemployedPops)
{
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 10000});

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});
	std::map<std::string, double> subsistenceEmployment{{"peasants", 90}, {"farmers", 10}};
	marketJobs.createSubsistence(subsistenceEmployment, .25, 0, 10, getPopTypes(), subState);

	const double delta = 0.001;
	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(500, delta)),
			  testing::Pair("laborers", testing::DoubleNear(3600, delta)),
			  testing::Pair("peasants", testing::DoubleNear(3600, delta)),
			  testing::Pair("farmers", testing::DoubleNear(400, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(1900, delta))));

	const double levels = marketJobs.createJobs({{"laborers", 100}, {"clerks", 100}}, subsistenceEmployment, 5, .25, 0, {}, {}, getPopTypes(), subState);

	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(237.5, delta)),
			  testing::Pair("laborers", testing::DoubleNear(3710, delta)),
			  testing::Pair("peasants", testing::DoubleNear(1710, delta)),
			  testing::Pair("clerks", testing::DoubleNear(2000, delta)),
			  testing::Pair("farmers", testing::DoubleNear(190, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(0, delta))));
	EXPECT_DOUBLE_EQ(5.25, levels);
}

TEST(V3World_MarketJobsTests, CreateJobsTakesFromPeasantsOnlyWhenNoUnemployed)
{
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 10000});

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});
	std::map<std::string, double> subsistenceEmployment{{"peasants", 90}, {"farmers", 10}};
	marketJobs.createSubsistence(subsistenceEmployment, .25, 0, 20, getPopTypes(), subState);

	const double delta = 0.001;
	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(617.284, delta)),
			  testing::Pair("laborers", testing::DoubleNear(4444.444, delta)),
			  testing::Pair("peasants", testing::DoubleNear(4444.444, delta)),
			  testing::Pair("farmers", testing::DoubleNear(493.828, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(0, delta))));

	const double levels = marketJobs.createJobs({{"laborers", 100}, {"clerks", 100}}, subsistenceEmployment, 5, .25, 0, {}, {}, getPopTypes(), subState);

	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(117.284, delta)),
			  testing::Pair("laborers", testing::DoubleNear(2844.444, delta)),
			  testing::Pair("peasants", testing::DoubleNear(844.444, delta)),
			  testing::Pair("clerks", testing::DoubleNear(2000, delta)),
			  testing::Pair("farmers", testing::DoubleNear(93.828, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(0, delta))));
	EXPECT_DOUBLE_EQ(10, levels);
}

TEST(V3World_MarketJobsTests, CreateJobsMakesFewerDependantsWithMoreEmpoweredWomen)
{
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 5000});

	constexpr double moreWomenWorking = 0.05;

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});
	std::map<std::string, double> subsistenceEmployment{{"peasants", 90}, {"farmers", 10}};
	marketJobs.createSubsistence(subsistenceEmployment, .25, moreWomenWorking, 5, getPopTypes(), subState);

	const double delta = 0.001;
	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(200, delta)),
			  testing::Pair("laborers", testing::DoubleNear(1500, delta)),
			  testing::Pair("peasants", testing::DoubleNear(1500, delta)),
			  testing::Pair("farmers", testing::DoubleNear(166.666, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(1633.333, delta))));

	const double levels = marketJobs.createJobs({{"laborers", 50}, {"clerks", 50}}, subsistenceEmployment, 3, .25, 0.05, {}, {}, getPopTypes(), subState);

	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(200, delta)),
			  testing::Pair("laborers", testing::DoubleNear(2000, delta)),
			  testing::Pair("peasants", testing::DoubleNear(1500, delta)),
			  testing::Pair("clerks", testing::DoubleNear(500, delta)),
			  testing::Pair("farmers", testing::DoubleNear(166.666, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(633.333, delta))));
	EXPECT_DOUBLE_EQ(0, levels);
}


TEST(V3World_MarketJobsTests, CreateJobsAddsAdditionalOwnershipJobs)
{
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 10000});

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});
	std::map<std::string, double> subsistenceEmployment{{"peasants", 90}, {"farmers", 10}};
	marketJobs.createSubsistence(subsistenceEmployment, .25, 0, 10, getPopTypes(), subState);

	const double delta = 0.001;
	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(500, delta)),
			  testing::Pair("laborers", testing::DoubleNear(3600, delta)),
			  testing::Pair("peasants", testing::DoubleNear(3600, delta)),
			  testing::Pair("farmers", testing::DoubleNear(400, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(1900, delta))));


	std::map<std::string, double> rgoEmployment{{"laborers", 100}, {"clerks", 100}};
	std::map<std::string, double> estOwnerships{{"building_financial_district", 0.5}, {"building_manor_house", 0.25}};
	std::map<std::string, int> capEmployment{{"capitalists", 10}};
	std::map<std::string, int> ariEmployment{{"aristocrats", 10}, {"laborers", 90}};

	std::map<std::string, std::map<std::string, int>> ownEmployments;
	ownEmployments.emplace("building_financial_district", capEmployment);
	ownEmployments.emplace("building_manor_house", ariEmployment);

	const double levels = marketJobs.createJobs(rgoEmployment, subsistenceEmployment, 5, .25, 0, estOwnerships, ownEmployments, getPopTypes(), subState);

	EXPECT_THAT(subState->getEstimatedJobs(),
		 testing::UnorderedElementsAre(testing::Pair("aristocrats", testing::DoubleNear(223.438, delta)),
			  testing::Pair("capitalists", testing::DoubleNear(100, delta)),
			  testing::Pair("laborers", testing::DoubleNear(3608.75, delta)),
			  testing::Pair("peasants", testing::DoubleNear(1158.75, delta)),
			  testing::Pair("clerks", testing::DoubleNear(2000, delta)),
			  testing::Pair("farmers", testing::DoubleNear(128.75, delta)),
			  testing::Pair("unemployed", testing::DoubleNear(0, delta))));
	EXPECT_DOUBLE_EQ(2712.5 / 400.0, levels);
}

TEST(V3World_MarketJobsTests, CreateJobsYieldsWarningIfASubsistenceBuildingEmploysNoPeasants)
{
	// Just peasants, no farmers
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 5000});

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});

	marketJobs.createSubsistence({{"farmers", 10}, {"peasants", 90}}, .25, 0, 5, getPopTypes(), subState);

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	marketJobs.createJobs({{"laborers", 50}, {"clerks", 50}}, {{"farmers", 100}}, 3, .25, 0.05, {}, {}, getPopTypes(), subState);

	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(),
		 testing::HasSubstr(R"([ERROR] Supposed subsistence building contains no peasants in its production methods. Job predictions will be unreliable.)"));
}

TEST(DISABLED_V3World_MarketJobsTests, CreateJobsNoWorkers)
{
	// Just peasants, no farmers
	auto subState = std::make_shared<V3::SubState>();
	subState->addPop({"", "", "", 5000});

	V3::MarketJobs marketJobs({{"aristocrats", 10}, {"laborers", 90}});

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	marketJobs.createJobs({{"laborers", 50}, {"clerks", 50}}, {{"peasants", 100}}, 3, .25, 0.05, {}, {}, getPopTypes(), subState);

	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] No subsistence workers available.)"));
}

// Arable land can change as farms/plantations are built.