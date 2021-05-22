#ifndef DATE_ITEMS_H
#define DATE_ITEMS_H
#include "Parser.h"
#include <vector>

namespace EU4
{
struct DateChange
{
	DateChange(const std::string& theChangeType, const std::string& theChangeValue)
	{
		changeType = theChangeType;
		changeValue = theChangeValue;
	}
	std::string changeType;
	std::string changeValue;
};

class DateItems: commonItems::parser
{
  public:
	explicit DateItems(std::istream& theStream);

	[[nodiscard]] const auto& getDateChanges() const { return dateChanges; }

  private:
	void registerKeys();

	std::vector<DateChange> dateChanges; // changeType, changeValue
};
} // namespace EU4

#endif // DATE_ITEMS_H