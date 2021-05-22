#include "ProvinceDetails/ProvinceDetailsMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_ProvinceDetailsMapperTests, detailsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::ProvinceDetailsMapper mapper(input);

	ASSERT_TRUE(mapper.getProvinceDetails().climate.empty());
	ASSERT_EQ(0, mapper.getProvinceDetails().colonial);
	ASSERT_EQ(0, mapper.getProvinceDetails().colonyLevel);
	ASSERT_TRUE(mapper.getProvinceDetails().controller.empty());
	ASSERT_TRUE(mapper.getProvinceDetails().cores.empty());
	ASSERT_EQ(0, mapper.getProvinceDetails().fortLevel);
	ASSERT_EQ(0, mapper.getProvinceDetails().lifeRating);
	ASSERT_EQ(0, mapper.getProvinceDetails().navalBaseLevel);
	ASSERT_TRUE(mapper.getProvinceDetails().owner.empty());
	ASSERT_EQ(0, mapper.getProvinceDetails().railLevel);
	ASSERT_TRUE(mapper.getProvinceDetails().rgoType.empty());
	ASSERT_FALSE(mapper.getProvinceDetails().slaveState);
	ASSERT_TRUE(mapper.getProvinceDetails().terrain.empty());
}

TEST(Mappers_ProvinceDetailsMapperTests, detailsCanBeLoaded)
{
	std::stringstream input;
	input << "colonial = 1\n";
	input << "colony = 2\n";
	input << "controller = TAG\n";
	input << "add_core = TA1\n";
	input << "add_core = TA2\n";
	input << "fort = 3\n";
	input << "life_rating = 30\n";
	input << "naval_base = 1\n";
	input << "owner = GAT\n";
	input << "railroad = 2\n";
	input << "trade_goods = socks\n";
	input << "is_slave = indeed\n";
	input << "terrain = rocks\n";
	const mappers::ProvinceDetailsMapper mapper(input);

	ASSERT_EQ(1, mapper.getProvinceDetails().colonial);
	ASSERT_EQ(2, mapper.getProvinceDetails().colonyLevel);
	ASSERT_EQ("TAG", mapper.getProvinceDetails().controller);
	ASSERT_EQ(2, mapper.getProvinceDetails().cores.size());
	ASSERT_TRUE(mapper.getProvinceDetails().cores.contains("TA1"));
	ASSERT_TRUE(mapper.getProvinceDetails().cores.contains("TA2"));
	ASSERT_EQ(3, mapper.getProvinceDetails().fortLevel);
	ASSERT_EQ(30, mapper.getProvinceDetails().lifeRating);
	ASSERT_EQ(1, mapper.getProvinceDetails().navalBaseLevel);
	ASSERT_EQ("GAT", mapper.getProvinceDetails().owner);
	ASSERT_EQ(2, mapper.getProvinceDetails().railLevel);
	ASSERT_EQ("socks", mapper.getProvinceDetails().rgoType);
	ASSERT_TRUE(mapper.getProvinceDetails().slaveState);
	ASSERT_EQ("rocks", mapper.getProvinceDetails().terrain);
}
