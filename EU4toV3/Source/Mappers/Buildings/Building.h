#ifndef BUILDING_H
#define BUILDING_H
#include "Parser.h"

namespace mappers
{
class Building: commonItems::parser
{
  public:
	explicit Building(std::istream& theStream);

	[[nodiscard]] auto getCost() const { return cost; }

  private:
	void registerKeys();

	double cost = 0;
};
} // namespace mappers

#endif // BUILDING_H