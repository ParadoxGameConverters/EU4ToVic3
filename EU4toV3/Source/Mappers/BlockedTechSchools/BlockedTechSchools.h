#ifndef BLOCKED_TECH_SCHOOLS_H
#define BLOCKED_TECH_SCHOOLS_H
#include "Parser.h"
#include <set>

namespace mappers
{
class BlockedTechSchools: commonItems::parser
{
  public:
	BlockedTechSchools();
	explicit BlockedTechSchools(std::istream& theStream);

	[[nodiscard]] bool isTechSchoolBlocked(const std::string& techSchool) const;

  private:
	void registerKeys();
	
	std::set<std::string> theBlockedTechSchools;
};
} // namespace mappers

#endif // BLOCKED_TECH_SCHOOLS_H