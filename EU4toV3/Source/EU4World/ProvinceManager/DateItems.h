#ifndef DATE_ITEMS
#define DATE_ITEMS
#include "DateChange.h"
#include "Parser.h"
#include <vector>

namespace EU4
{
class DateItems: commonItems::parser
{
  public:
	explicit DateItems(std::istream& theStream);

	[[nodiscard]] const auto& getDateChanges() const { return dateChanges; }

  private:
	void registerKeys();

	std::vector<DateChange> dateChanges;
};
} // namespace EU4

#endif // DATE_ITEMS