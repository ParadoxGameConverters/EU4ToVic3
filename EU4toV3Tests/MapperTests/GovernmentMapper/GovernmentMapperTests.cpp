#include "GovernmentMapper/GovernmentMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_GovernmentMapperTests, governmentCanBeMatched)
{
	std::stringstream input;
	input << "destgov1 = { sgov1 sgov2 }\n";
	input << "destgov3 = { sgov3 }\n";
	const mappers::GovernmentMapper mapper(input);

	ASSERT_EQ("destgov3", *mapper.matchGovernment("sgov3"));
}

TEST(Mappers_GovernmentMapperTests, mismatchReturnsNulloptAndAStarkWarning)
{
	std::stringstream input;
	input << "destgov1 = { sgov1 sgov2 }\n";
	input << "destgov3 = { sgov3 }\n";
	const mappers::GovernmentMapper mapper(input);

	const std::stringstream log;
	auto* stdOutBuf = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	ASSERT_EQ(std::nullopt, mapper.matchGovernment("sgov-error"));
	std::cout.rdbuf(stdOutBuf);
	
	ASSERT_EQ(" [WARNING] No government mapping defined for sgov-error!\n", log.str());
}
