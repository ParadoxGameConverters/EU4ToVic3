#ifndef COUNTRY_MAPPING_H
#define COUNTRY_MAPPING_H
#include "Parser.h"
#include <set>

namespace mappers
{
class CountryMapping: commonItems::parser
{
  public:
	CountryMapping() = default;
	explicit CountryMapping(std::istream& theStream);

	[[nodiscard]] const auto& getEU4Tag() const { return eu4Tag; }
	[[nodiscard]] const auto& getV3Tag() const { return v3Tag; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getFlagCode() const { return flagCode; }
	[[nodiscard]] const auto& getReforms() const { return reforms; }
	[[nodiscard]] const auto& getFlags() const { return flags; }

  private:
	void registerKeys();

	std::optional<std::string> eu4Tag;
	std::optional<std::string> v3Tag;
	std::optional<std::string> name;
	std::optional<std::string> flagCode;
	std::set<std::string> reforms;
	std::set<std::string> flags;
};
} // namespace mappers

#endif // COUNTRY_MAPPING_H