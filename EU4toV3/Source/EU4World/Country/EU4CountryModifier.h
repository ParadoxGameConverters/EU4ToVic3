#ifndef EU4_COUNTRY_MODIFIER_H
#define EU4_COUNTRY_MODIFIER_H
#include "Parser.h"

namespace EU4
{
class EU4CountryModifier: commonItems::parser
{
  public:
	EU4CountryModifier() = default;
	explicit EU4CountryModifier(std::istream& theStream);

	[[nodiscard]] const auto& getModifier() const { return modifier; }

  private:
	void registerKeys();

	std::string modifier;
};
} // namespace EU4

#endif // EU4_COUNTRY_MODIFIER_H