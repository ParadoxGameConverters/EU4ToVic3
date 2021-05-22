#ifndef COLONIAL_FLAG_H
#define COLONIAL_FLAG_H
#include "Parser.h"
#include <string>

namespace mappers
{
class ColonialFlag: commonItems::parser
{
  public:
	ColonialFlag() = default;
	ColonialFlag(std::istream& theStream, const std::string& theRegion);

	[[nodiscard]] auto isUnique() const { return unique; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getRegion() const { return region; }
	[[nodiscard]] const auto& getOverlord() const { return overlord; }

	void setOverlord(const std::string& newOverlord) { overlord = newOverlord; }

  private:
	void registerKeys();

	bool unique = false;
	std::string name;
	std::string region;
	std::string overlord;
};
} // namespace mappers

#endif // COLONIAL_FLAG_H
