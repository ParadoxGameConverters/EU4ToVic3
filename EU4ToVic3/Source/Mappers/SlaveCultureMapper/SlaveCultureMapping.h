#ifndef SLAVE_CULTURE_MAPPING_H
#define SLAVE_CULTURE_MAPPING_H
#include "Parser.h"

namespace mappers
{
class SlaveCultureMapping: commonItems::parser
{
  public:
	SlaveCultureMapping() = default;
	explicit SlaveCultureMapping(std::istream& theStream);

	[[nodiscard]] const auto& getMasterTrait() const { return masterTrait; }
	[[nodiscard]] const auto& getSlaveCulture() const { return slaveCulture; }

  private:
	void registerKeys();

	std::string masterTrait;
	std::string slaveCulture;
};
} // namespace mappers

#endif // SLAVE_CULTURE_MAPPING_H