#ifndef RELIGIONPARSER
#define RELIGIONPARSER
#include "Parser.h"
#include "Religion.h"

namespace EU4
{
class ReligionParser: commonItems::parser
{
  public:
	ReligionParser(std::istream& theStream);
	[[nodiscard]] const auto& getReligions() const { return religions; }

  private:
	void registerKeys();

	std::vector<Religion> religions;
};
} // namespace EU4

#endif // RELIGIONPARSER