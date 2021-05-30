#ifndef RELIGIONPARSER
#define RELIGIONPARSER
#include "Parser.h"
#include <set>

namespace EU4
{
class ReligionParser: commonItems::parser
{
  public:
	ReligionParser(std::istream& theStream);
	auto takeReligions() { return std::move(religions); }

  private:
	void registerKeys();

	std::set<std::string> religions;
};
} // namespace EU4

#endif // RELIGIONPARSER