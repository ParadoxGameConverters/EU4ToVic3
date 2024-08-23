#include "EconomyManager/Packet.h"
#include <gtest/gtest.h>

TEST(V3World_PacketTests, ZeroInputReturnsMinimumPacket)
{
	V3::Building b;
	V3::SubState s;
	V3::BuildingGroups bgs;

	EXPECT_EQ(1, V3::Packet(b, 0, 0, s, {}, {}, {}, {}, bgs).getSize());
}