#include "ClayManager/State/State.h"
#include "EconomyManager/Packet.h"
#include "PoliticalManager/Country/Country.h"

#include <gtest/gtest.h>

TEST(V3World_PacketTests, ZeroInputReturnsMinimumPacket)
{
	V3::Building b;
	V3::SubState s;
	V3::BuildingGroups bgs;

	EXPECT_EQ(1, V3::Packet(b, 0, 0, s, {}, {}, {}, {}, bgs).getSize());
}

TEST(V3World_PacketTests, SectorPacket)
{
	V3::Building b;
	auto s = std::make_shared<V3::SubState>();
	V3::BuildingGroups bgs;

	std::stringstream buildingInput;
	buildingInput << "required_construction = 30";
	b.loadBuilding(buildingInput, {});

	s->setCPBudget(INT_MAX);


	EXPECT_EQ(4, V3::Packet(b, 120, 1, *s, {s}, {}, {}, {}, bgs).getSize());
}

TEST(V3World_PacketTests, SubStatePacket)
{
	V3::Building b;
	auto s = std::make_shared<V3::SubState>();
	V3::BuildingGroups bgs;

	std::stringstream buildingInput;
	buildingInput << "required_construction = 30";
	b.loadBuilding(buildingInput, {});

	s->setCPBudget(150);


	EXPECT_EQ(5, V3::Packet(b, INT_MAX, 1, *s, {s}, {}, {}, {}, bgs).getSize());
}

TEST(V3World_PacketTests, CapacityPacket)
{
	V3::BuildingGroups bgs;

	auto c = std::make_shared<V3::Country>();
	V3::ProcessedData data;
	data.techs = {"t"};
	c->setProcessedData(data);

	auto hs = std::make_shared<V3::State>();

	V3::Building b;
	std::stringstream buildingInput;
	buildingInput << "required_construction = 30\n";
	buildingInput << "has_max_level = yes\n";
	b.loadBuilding(buildingInput, {});
	b.setName("b");

	auto s = std::make_shared<V3::SubState>();
	s->setCPBudget(INT_MAX);
	s->setOwner(c);
	s->setHomeState(hs);

	V3::Tech t;
	t.maxBuildingLevels["b"] = 2;


	EXPECT_EQ(2, V3::Packet(b, INT_MAX, 1, *s, {s}, {}, {{"t", t}}, {}, bgs).getSize());
}

TEST(V3World_PacketTests, ClusterPacketMinimumFactor)
{
	V3::Building b;
	auto s = std::make_shared<V3::SubState>();
	auto s2 = std::make_shared<V3::SubState>();
	V3::BuildingGroups bgs;

	std::stringstream buildingInput;
	buildingInput << "required_construction = 30";
	b.loadBuilding(buildingInput, {});

	s->setCPBudget(INT_MAX - 90);
	s2->setCPBudget(90);


	EXPECT_EQ(3, V3::Packet(b, INT_MAX, -1, *s, {s, s2}, {}, {}, {}, bgs).getSize());
}

TEST(V3World_PacketTests, ClusterPacketZeroFactor)
{
	V3::Building b;
	auto s = std::make_shared<V3::SubState>();
	auto s2 = std::make_shared<V3::SubState>();
	V3::BuildingGroups bgs;

	std::stringstream buildingInput;
	buildingInput << "required_construction = 30";
	b.loadBuilding(buildingInput, {});

	s->setCPBudget(INT_MAX - 90);
	s2->setCPBudget(90);


	EXPECT_EQ(35791394, V3::Packet(b, INT_MAX, 0, *s, {s, s2}, {}, {}, {}, bgs).getSize());
}