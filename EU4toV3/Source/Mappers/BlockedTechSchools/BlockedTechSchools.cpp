#include "BlockedTechSchools.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::BlockedTechSchools::BlockedTechSchools()
{
	registerKeys();
	parseFile("configurables/blocked_tech_schools.txt");
	clearRegisteredKeywords();
}

mappers::BlockedTechSchools::BlockedTechSchools(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::BlockedTechSchools::registerKeys()
{
	registerKeyword("blocked", [this](const std::string& unused, std::istream& theStream) {
		theBlockedTechSchools.insert(commonItems::singleString(theStream).getString());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool mappers::BlockedTechSchools::isTechSchoolBlocked(const std::string& techSchool) const
{
	return theBlockedTechSchools.contains(techSchool);
}
