#ifndef COLONIAL_FLAG_REGION_H
#define COLONIAL_FLAG_REGION_H
#include "ColonialFlag.h"
#include "Parser.h"
#include <map>

namespace mappers
{
class ColonialFlagRegion: commonItems::parser
{
  public:
	ColonialFlagRegion(std::istream& theStream, const std::string& region);

	[[nodiscard]] const auto& getRegionalFlags() const { return regionalFlags; }

  private:
	void registerKeys(const std::string& region);

	std::map<std::string, ColonialFlag> regionalFlags;
};
} // namespace mappers

#endif // COLONIAL_FLAG_REGION_H
