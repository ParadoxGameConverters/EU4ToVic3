#include "AssignmentMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::AssignmentMapping::AssignmentMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::AssignmentMapping::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& assignment, std::istream& theStream) {
		assignments.emplace(assignment, commonItems::getString(theStream));
	});
}
