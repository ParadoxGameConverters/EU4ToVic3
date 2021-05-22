#ifndef UNIT_TYPE_H
#define UNIT_TYPE_H
#include "Parser.h"

namespace mappers
{
class UnitType: commonItems::parser
{
  public:
	UnitType() = default;
	explicit UnitType(std::istream& theStream);
	explicit UnitType(const std::string& filePath);

	void setStrength(const int theStrength) { strength = theStrength; }

	[[nodiscard]] auto getCategory() const { return unitType; }
	[[nodiscard]] auto getStrength() const { return strength; }

  private:
	void registerKeys();

	std::string unitType;
	int strength = 0;
};
}; // namespace mappers

#endif // UNIT_TYPE_H
