#ifndef UNIT_TYPE_PARSER
#define UNIT_TYPE_PARSER
#include "Parser.h"
#include "UnitType.h"
#include <filesystem>

namespace EU4
{
class UnitTypeParser: commonItems::parser
{
  public:
	UnitTypeParser() = default;
	explicit UnitTypeParser(std::istream& theStream);
	explicit UnitTypeParser(const std::filesystem::path& filePath);

	[[nodiscard]] const auto& getUnitType() const { return unitType; }

  private:
	void registerKeys();

	UnitType unitType;
};
}; // namespace EU4

#endif // UNIT_TYPE_PARSER
