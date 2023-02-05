#ifndef EU4RELATION_DETAILS_H
#define EU4RELATION_DETAILS_H
#include "ConvenientParser.h"
#include "Date.h"

namespace EU4
{
class EU4RelationDetails: commonItems::convenientParser
{
  public:
	EU4RelationDetails() = default;
	explicit EU4RelationDetails(std::istream& theStream);

	[[nodiscard]] auto getRelations() const { return value; }
	void setValue(int theValue) { value = theValue; }
	[[nodiscard]] auto hasMilitaryAccess() const { return military_access; }
	[[nodiscard]] const auto& getAttitude() const { return attitude; }
	[[nodiscard]] std::optional<date> getTruceExpiry() const;

  private:
	void registerKeys();

	int value = 0;
	bool military_access = false;
	std::string attitude = "attitude_neutral";
	date lastWar;
	int lastWarScore = 0;
	bool truce = false;
};
} // namespace EU4

#endif // EU4RELATION_DETAILS_H