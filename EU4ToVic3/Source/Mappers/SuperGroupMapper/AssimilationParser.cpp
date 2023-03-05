#include "AssimilationParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::AssimilationParser::AssimilationParser(std::istream& theStream)
{
	registerSetter("assimilation", assimilation);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	parseStream(theStream);
	clearRegisteredKeywords();
}
