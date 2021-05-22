#include "UnitTypes/UnitType.h"
#include "UnitTypes/UnitTypeMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_UnitTypeMapperTests, UnitTypesCanBeMatched)
{
	mappers::UnitTypeMapper unitTypeMapper;

	std::stringstream input;
	input << "type = cavalry\n";
	input << "maneuver = 2\n";
	unitTypeMapper.loadUnitType("some-cavalry", input);
	std::stringstream input2;
	input2 << "type = infantry\n";
	input2 << "maneuver = 4\n";
	unitTypeMapper.loadUnitType("some-infantry", input2);

	const auto unitType1 = unitTypeMapper.getUnitTypeForRegimentTypeName("some-cavalry");
	ASSERT_EQ("cavalry", unitType1->getCategory());
	ASSERT_EQ(2, unitType1->getStrength());

	const auto unitType2 = unitTypeMapper.getUnitTypeForRegimentTypeName("some-infantry");
	ASSERT_EQ("infantry", unitType2->getCategory());
	ASSERT_EQ(4, unitType2->getStrength());
}

TEST(Mappers_UnitTypeMapperTests, UnitTypeMismatchReturnsNull)
{
	mappers::UnitTypeMapper unitTypeMapper;

	std::stringstream input;
	input << "type = cavalry\n";
	input << "maneuver = 2\n";
	unitTypeMapper.loadUnitType("some-cavalry", input);
	std::stringstream input2;
	input2 << "type = infantry\n";
	input2 << "maneuver = 4\n";
	unitTypeMapper.loadUnitType("some-infantry", input2);

	const auto unitType = unitTypeMapper.getUnitTypeForRegimentTypeName("some-junk");
	ASSERT_FALSE(unitType);
}

TEST(Mappers_UnitTypeMapperTests, TransportUnitTypeIsSetTo24Strength)
{
	mappers::UnitTypeMapper unitTypeMapper;

	std::stringstream input;
	input << "type = transport\n";
	input << "maneuver = 2\n";
	unitTypeMapper.loadUnitType("some-transport", input);

	const auto unitType = unitTypeMapper.getUnitTypeForRegimentTypeName("some-transport");
	ASSERT_EQ(24, unitType->getStrength());
}
