#ifndef LAW_ENTRY_H
#define LAW_ENTRY_H
#include "PMRule.h"
#include "Parser.h"

namespace mappers
{
class ProductionMethodEntry: commonItems::parser
{

  public:
	ProductionMethodEntry() = default;
	explicit ProductionMethodEntry(std::istream& theStream);

	[[nodiscard]] const auto& getRule() const { return rule; }

  private:
	void registerKeys();

	PMRule rule;
};
} // namespace mappers
#endif // LAW_ENTRY_H