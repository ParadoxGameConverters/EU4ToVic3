#include "ExcludedTradeCompaniesMapper/ExcludedTradeCompaniesMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_ExcludedTradeCompaniesMapperTests, DefaultsDefaultToDefaults)
{
	const mappers::ExcludedTradeCompaniesMapper mapper;

	EXPECT_TRUE(mapper.getExcludedTCs().empty());
}

TEST(Mappers_ExcludedTradeCompaniesMapperTests, TCsCanBeLoaded)
{
	mappers::ExcludedTradeCompaniesMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/excluded_trade_companies.txt");

	ASSERT_EQ(10, mapper.getExcludedTCs().size());
	EXPECT_TRUE(mapper.getExcludedTCs().contains("trade_company_west_africa"));
}
