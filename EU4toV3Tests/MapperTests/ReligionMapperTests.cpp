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
#include "../EU4toV3/Source/Mappers/ReligionMapper/ReligionMapper.h"
#include <sstream>



TEST(Mappers_ReligionMapperTests, nonMatchGivesEmptyOptional)
{
	std::stringstream input;
	input << "link = { v2 = vic2Religion eu4 = eu4Religion }";

	mappers::ReligionMapper theMapper(input);

	std::optional<std::string> vic2Religion = theMapper.getVic2Religion("nonMatchingReligion");
	ASSERT_FALSE(vic2Religion);
}


TEST(Mappers_ReligionMapperTests, vic2ReligionCanBeFound)
{
	std::stringstream input;
	input << "link = { v2 = vic2Religion eu4 = eu4Religion }";

	mappers::ReligionMapper theMapper(input);

	std::optional<std::string> vic2Religion = theMapper.getVic2Religion("eu4Religion");
	ASSERT_EQ(vic2Religion, "vic2Religion");
}


TEST(Mappers_ReligionMapperTests, multipleEU4ReligionsCanBeInARule)
{
	std::stringstream input;
	input << "link = { v2 = vic2Religion eu4 = eu4Religion eu4 = eu4Religion2 }";

	mappers::ReligionMapper theMapper(input);

	std::optional<std::string> vic2Religion = theMapper.getVic2Religion("eu4Religion2");
	ASSERT_EQ(vic2Religion, "vic2Religion");
}


TEST(Mappers_ReligionMapperTests, correctRuleMatches)
{
	std::stringstream input;
	input << "link = { v2 = vic2Religion eu4 = eu4Religion }";
	input << "link = { v2 = vic2Religion2 eu4 = eu4Religion2 }";

	mappers::ReligionMapper theMapper(input);

	std::optional<std::string> vic2Religion = theMapper.getVic2Religion("eu4Religion2");
	ASSERT_EQ(vic2Religion, "vic2Religion2");
}