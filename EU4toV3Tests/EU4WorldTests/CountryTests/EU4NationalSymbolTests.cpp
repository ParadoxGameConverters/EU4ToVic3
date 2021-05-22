#include "Country/EU4NationalSymbol.h"
#include "gtest/gtest.h"

TEST(EU4World_Country_EU4NationalSymbol, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::NationalSymbol symbol(input);

	ASSERT_FALSE(symbol.getCustomColors());
	ASSERT_FALSE(symbol.getMapColor());
	ASSERT_FALSE(symbol.getRevolutionaryColor());
}

TEST(EU4World_Country_EU4NationalSymbol, colorsCanBeLoaded)
{
	std::stringstream input;
	input << "map_color = { 1 2 3 }\n";
	input << "revolutionary_colors = { 4 5 6 }\n";
	input << "custom_colors = {\n";
	input << "\tflag = 10\n";
	input << "}\n";
	const EU4::NationalSymbol symbol(input);

	ASSERT_EQ("= rgb { 1 2 3 }", symbol.getMapColor()->outputRgb());
	ASSERT_EQ("= rgb { 4 5 6 }", symbol.getRevolutionaryColor()->outputRgb());
	ASSERT_EQ(11, symbol.getCustomColors()->getCustomColors().flag); // A tad odd syntax but it's there for legacy reasons. Maybe.
}
