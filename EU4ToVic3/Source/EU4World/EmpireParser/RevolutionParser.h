#ifndef REVOLUTION_PARSER
#define REVOLUTION_PARSER
#include "Parser.h"
#include <set>

namespace EU4
{
class RevolutionParser: commonItems::parser
{
  public:
	explicit RevolutionParser(std::istream& theStream);

	[[nodiscard]] const auto& getRevolutionTarget() const { return revolutionTarget; }

  private:
	void registerKeywords();

	std::string revolutionTarget;
};
} // namespace EU4

#endif // REVOLUTION_PARSER