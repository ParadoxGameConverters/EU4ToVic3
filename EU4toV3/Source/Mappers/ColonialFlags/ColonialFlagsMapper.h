#ifndef COLONIAL_FLAGS_MAPPER
#define COLONIAL_FLAGS_MAPPER
#include "ColonialFlagRegion.h"
#include "Parser.h"
#include <map>

namespace mappers
{
class ColonialFlagsMapper: commonItems::parser
{
  public:
	ColonialFlagsMapper();
	explicit ColonialFlagsMapper(std::istream& theStream);

	[[nodiscard]] std::optional<ColonialFlag> getFlag(const std::string& name) const;
	[[nodiscard]] std::vector<std::string> getCommonNames() const;

	void removeFlag(const std::string& name);

  private:
	void registerKeys();

	std::map<std::string, ColonialFlag> colonialFlags;
};
} // namespace mappers

#endif // COLONIAL_FLAGS_MAPPER
