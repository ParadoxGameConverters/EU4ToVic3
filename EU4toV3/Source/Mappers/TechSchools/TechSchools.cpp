#include "TechSchools.h"
#include "Log.h"

mappers::TechSchools::TechSchools(std::istream& theStream)
{
	registerRegex("[a-zA-Z0-9_]+", [this](const std::string& name, std::istream& theStream)
		{
			TechSchool newTechSchool(theStream);
			if (!blockedTechSchools.isTechSchoolBlocked(name)) techSchools.insert(std::make_pair(name, newTechSchool));
		});

	parseStream(theStream);
	clearRegisteredKeywords();
}
