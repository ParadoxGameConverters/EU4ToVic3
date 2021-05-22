#ifndef RELIGION_GROUP
#define RELIGION_GROUP
#include "Parser.h"
#include <set>

namespace EU4
{
class ReligionGroup: commonItems::parser
{
  public:
	ReligionGroup(std::istream& theStream);
	auto takeReligions() { return std::move(religions); }

  private:
	void registerKeys();

	std::set<std::string> religions;
};
} // namespace EU4

#endif // RELIGION_GROUP