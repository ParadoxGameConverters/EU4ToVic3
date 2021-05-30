#include "CountryManager/NationalSymbol/EU4NationalSymbol.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4NationalSymbolTests, primitivesDefaultToFalse)
{
	std::stringstream input;
	const EU4::NationalSymbol symbol(input);

	EXPECT_FALSE(symbol.getCustomColors());
	EXPECT_FALSE(symbol.getMapColor());
	EXPECT_FALSE(symbol.getRevolutionaryColor());
}

TEST(EU4World_EU4NationalSymbolTests, colorsCanBeLoaded)
{
	std::stringstream input;
	input << "map_color = { 1 2 3 }\n";
	input << "revolutionary_colors = { 4 5 6 }\n";
	input << "custom_colors = {\n";
	input << "\tflag = 10\n";
	input << "}\n";
	const EU4::NationalSymbol symbol(input);

	EXPECT_EQ("= rgb { 1 2 3 }", symbol.getMapColor()->outputRgb());
	EXPECT_EQ("= rgb { 4 5 6 }", symbol.getRevolutionaryColor()->outputRgb());
	EXPECT_EQ(11, symbol.getCustomColors()->flagIndex);
}
