#ifndef UNIT_TYPE_MAPPER_H
#define UNIT_TYPE_MAPPER_H
#include "Parser.h"
#include "UnitTypes/UnitType.h"

namespace mappers
{
class UnitTypeMapper
{
  public:
	UnitTypeMapper() = default;
	void initUnitTypeMapper();																 // production
	void loadUnitType(const std::string& unitName, std::istream& theStream); // testing

	[[nodiscard]] std::optional<UnitType> getUnitTypeForRegimentTypeName(const std::string& regimentTypeName) const;

  private:
	std::map<std::string, UnitType> unitTypeMap;
	void addUnitFileToRegimentTypeMap(const std::string& directory, const std::string& filename);
};
}; // namespace mappers

#endif // UNIT_TYPE_MAPPER_H
