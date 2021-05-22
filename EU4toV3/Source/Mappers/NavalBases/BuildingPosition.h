#ifndef BUILDING_POSITION_H
#define BUILDING_POSITION_H
#include "Parser.h"

namespace mappers
{
class BuildingPosition: commonItems::parser
{
  public:
	BuildingPosition() = default;
	explicit BuildingPosition(std::istream& theStream);

	[[nodiscard]] auto isFound() const { return found; }

  private:
	void registerKeys();

	bool found = false;
};
} // namespace mappers

#endif // BUILDING_POSITION_H
