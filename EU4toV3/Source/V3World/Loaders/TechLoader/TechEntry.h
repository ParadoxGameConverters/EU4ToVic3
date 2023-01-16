#ifndef TECH_ENTRY_H
#define TECH_ENTRY_H
#include "Parser.h"
#include "Tech.h"

namespace V3
{
class TechEntry: commonItems::parser
{

  public:
	TechEntry() = default;
	explicit TechEntry(std::istream& theStream);

	[[nodiscard]] const auto& getTech() const { return tech; }

  private:
	void registerKeys();

	parser modifierParser;

	Tech tech;
};
} // namespace V3
#endif // TECH_ENTRY_H