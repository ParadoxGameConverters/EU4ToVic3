#ifndef FACTORY_STARTING_COUNTS_H
#define FACTORY_STARTING_COUNTS_H
#include "Parser.h"

namespace mappers
{
class FactoryStartingCounts: commonItems::parser
{
  public:
	FactoryStartingCounts();
	explicit FactoryStartingCounts(std::istream& theStream);

	[[nodiscard]] std::optional<int> getCountForFactoryType(const std::string& factoryType) const;

  private:
	void registerKeywords();

	std::map<std::string, int> startingCounts;
};
} // namespace mappers

#endif // FACTORY_STARTING_COUNTS_H