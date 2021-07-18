#ifndef DATE_CHANGE
#define DATE_CHANGE
#include "Parser.h"

namespace EU4
{
struct DateChange
{
	DateChange(const std::string& theChangeType, const std::string& theChangeValue)
	{
		changeType = theChangeType;
		changeValue = theChangeValue;
	}
	bool operator==(const DateChange& rhs) const { return changeType == rhs.changeType && changeValue == rhs.changeValue; }

	std::string changeType;
	std::string changeValue;
};

} // namespace EU4

#endif // DATE_CHANGE