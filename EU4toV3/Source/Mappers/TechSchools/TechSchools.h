#ifndef TECH_SCHOOLS
#define TECH_SCHOOLS

#include "TechSchool.h"
#include "../BlockedTechSchools/BlockedTechSchools.h"
#include "Parser.h"
#include <map>

namespace mappers
{
	class TechSchools: commonItems::parser
	{
	public:
		explicit TechSchools(std::istream& theStream);
		
		[[nodiscard]] const auto& getTechSchools() const { return techSchools; }

	private:
		std::map<std::string, TechSchool> techSchools;
		BlockedTechSchools blockedTechSchools;
	};
}

#endif // TECH_SCHOOLS