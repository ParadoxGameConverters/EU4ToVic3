#ifndef EU4_REFORM_STACK_SECTION_H
#define EU4_REFORM_STACK_SECTION_H
#include "Parser.h"
#include <set>

namespace EU4
{
class ReformStackSection: commonItems::parser
{
  public:
	explicit ReformStackSection(std::istream& theStream);

	[[nodiscard]] const auto& getReforms() const { return reforms; }

  private:
	void registerKeys();

	std::set<std::string> reforms;
};
} // namespace EU4

#endif // EU4_REFORM_STACK_SECTION_H
