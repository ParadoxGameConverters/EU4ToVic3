/*Copyright (c) 2019 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "gtest/gtest.h"
#include "../EU4toV3/Source/Mappers/ReligionMapper/ReligionMapping.h"
#include <sstream>



TEST(Mappers_ReligionMappingTests, vic2ReligionDefaultsToBlank)
{
	std::stringstream input;
	input << "= {}";

	mappers::ReligionMapping theMapping(input);

	ASSERT_EQ(theMapping.getVic2Religion(), "");
}


TEST(Mappers_ReligionMappingTests, vic2ReligionCanBeSet)
{
	std::stringstream input;
	input << "= { v2 = vic2Religion }";

	mappers::ReligionMapping theMapping(input);

	ASSERT_EQ(theMapping.getVic2Religion(), "vic2Religion");
}


TEST(Mappers_ReligionMappingTests, eu4ReligionsDefaultToEmpty)
{
	std::stringstream input;
	input << "= {}";

	mappers::ReligionMapping theMapping(input);

	ASSERT_EQ(theMapping.getEU4Religions().size(), 0);
}


TEST(Mappers_ReligionMappingTests, eu4ReligionsCanBeSet)
{
	std::stringstream input;
	input << "= { eu4 = religion1 eu4 = religion2 }";

	mappers::ReligionMapping theMapping(input);

	ASSERT_EQ(theMapping.getEU4Religions().size(), 2);
	ASSERT_EQ(theMapping.getEU4Religions()[0], "religion1");
	ASSERT_EQ(theMapping.getEU4Religions()[1], "religion2");
}