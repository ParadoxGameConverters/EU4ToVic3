#ifndef EU4RELATION_DETAILS_H
#define EU4RELATION_DETAILS_H
#include "Parser.h"

namespace EU4
{
class EU4RelationDetails: commonItems::parser
{
  public:
	explicit EU4RelationDetails(std::istream& theStream);

	[[nodiscard]] auto getRelations() const { return value; }
	[[nodiscard]] auto hasMilitaryAccess() const { return military_access; }
	[[nodiscard]] const auto& getAttitude() const { return attitude; }

  private:
	void registerKeys();

	int value = 0;
	bool military_access = false;
	std::string attitude = "attitude_neutral";
};
} // namespace EU4

#endif // EU4RELATION_DETAILS_H