#ifndef EU4_GOVERNMENT_SECTION_H
#define EU4_GOVERNMENT_SECTION_H
#include "Parser.h"
#include <set>

namespace EU4
{
class GovernmentSection: commonItems::parser
{
  public:
	explicit GovernmentSection(std::istream& theStream);

	[[nodiscard]] const auto& getGovernment() const { return government; }
	[[nodiscard]] const auto& getGovernmentReforms() const { return reformStack; }

  private:
	void registerKeys();

	std::string government;
	std::set<std::string> reformStack;
};
} // namespace EU4

#endif // EU4_GOVERNMENT_SECTION_H
