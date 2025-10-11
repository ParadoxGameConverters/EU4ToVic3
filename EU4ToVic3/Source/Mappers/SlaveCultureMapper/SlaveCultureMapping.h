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

	[[nodiscard]] const auto& getMasterHeritage() const { return masterHeritage; }
	[[nodiscard]] const auto& getMasterLanguage() const { return masterLanguage; }
	[[nodiscard]] const auto& getSlaveCulture() const { return slaveCulture; }

  private:
	void registerKeys();

	std::optional<std::string> masterHeritage;
	std::optional<std::string> masterLanguage;
	std::string slaveCulture;
};
} // namespace mappers

#endif // SLAVE_CULTURE_MAPPING_H