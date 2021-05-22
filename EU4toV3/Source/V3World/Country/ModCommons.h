#ifndef MOD_COMMONS_H
#define MOD_COMMONS_H

#include "../Party/Party.h"
#include "Color.h"
#include "Parser.h"

namespace V2
{
class ModCommons: commonItems::parser
{
  public:
	ModCommons() = default;
	ModCommons(const std::string& filename);

	void setPartyDates();

	[[nodiscard]] const auto& getColorString() const { return colorString; }
	[[nodiscard]] const auto& getGraphicalCulture() const { return graphicalCulture; }
	[[nodiscard]] const auto& getParties() const { return parties; }
	[[nodiscard]] const auto& getUnitNames() const { return unitNames; }

  private:
	std::string colorString;
	std::string graphicalCulture;
	std::vector<Party> parties;
	std::string unitNames;
};
} // namespace V2


#endif // MOD_COMMONS_H