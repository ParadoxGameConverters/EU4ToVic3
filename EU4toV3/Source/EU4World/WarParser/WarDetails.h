#ifndef WAR_DETAILS
#define WAR_DETAILS
#include "ConvenientParser.h"
#include "Date.h"

namespace EU4
{
class WarDetails: commonItems::convenientParser
{
  public:
	WarDetails() = default;
	void addDetails(std::istream& theStream);

	// This is a storage container shared between EU4 and V3. Because war never changes.
	int targetProvinceID = 0;
	std::string warGoalType;
	std::string targetTag;
	std::string warGoalClass;
	date startDate;

  private:
	void registerKeys();
};
} // namespace EU4

#endif // WAR_DETAILS