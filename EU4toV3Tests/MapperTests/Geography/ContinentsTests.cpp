#include "Geography/Continents.h"
#include "gtest/gtest.h"

TEST(Mappers_Geography_ContinentsTests, continentCanBePinged)
{
	std::stringstream input;
	input << "europe = { 1 2 3 }\n";
	input << "asia = { 7 8 9 }\n";
	const mappers::Continents mapper(input);

	ASSERT_EQ("europe", *mapper.getEU4Continent(2));
}

TEST(Mappers_Geography_ContinentsTests, mispingReturnsNullopt)
{
	std::stringstream input;
	input << "europe = { 1 2 3 }\n";
	input << "asia = { 7 8 9 }\n";
	const mappers::Continents mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getEU4Continent(6));
}
