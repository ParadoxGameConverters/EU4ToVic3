#ifndef EMPIRE_PARSER
#define EMPIRE_PARSER
#include "Parser.h"
#include <set>

namespace EU4
{
class EmpireParser: commonItems::parser
{
  public:
	explicit EmpireParser(std::istream& theStream);

	[[nodiscard]] const auto& getEmperor() const { return emperor; }
	[[nodiscard]] const auto& getHREReforms() const { return reforms; }

  private:
	void registerKeywords();

	std::string emperor;
	std::set<std::string> reforms;
};
} // namespace EU4

#endif // EMPIRE_PARSER