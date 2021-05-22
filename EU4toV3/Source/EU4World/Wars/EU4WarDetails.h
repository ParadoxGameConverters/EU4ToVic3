#ifndef EU4WAR_DETAILS_H
#define EU4WAR_DETAILS_H
#include "Date.h"
#include "ConvenientParser.h"

namespace EU4
{
class WarDetails: commonItems::convenientParser
{
  public:
	WarDetails() = default;
	void addDetails(std::istream& theStream);

	// This is a storage container shared between EU4 and V2. Because war never changes.
	int targetProvinceID = 0;
	std::string warGoalType;
	std::string targetTag;
	std::string warGoalClass;
	date startDate;

  private:
	void registerKeys();
};
} // namespace EU4

#endif // EU4WAR_DETAILS_H