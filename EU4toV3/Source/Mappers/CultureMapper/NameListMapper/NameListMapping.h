#ifndef NAME_LIST_MAPPING_H
#define NAME_LIST_MAPPING_H
#include "Parser.h"

namespace mappers
{
class NameListMapping: commonItems::parser
{
  public:
	explicit NameListMapping(std::istream& theStream);

	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getCultureGroup() const { return cultureGroup; }
	[[nodiscard]] const auto& getNamePool() const { return namePool; }

  private:
	void registerkeys();

	std::string culture;
	std::string cultureGroup;
	std::string namePool;
};
} // namespace mappers

#endif // NAME_LIST_MAPPING_H