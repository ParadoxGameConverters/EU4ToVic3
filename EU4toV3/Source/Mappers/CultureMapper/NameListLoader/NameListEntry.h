#ifndef NAME_LIST_ENTRY_H
#define NAME_LIST_ENTRY_H
#include "Parser.h"

namespace mappers
{
class NameListEntry: commonItems::parser
{
  public:
	explicit NameListEntry(std::istream& theStream);

	[[nodiscard]] const auto& getDynastyNames() const { return dynastyNames; }
	[[nodiscard]] const auto& getMaleNames() const { return maleNames; }
	[[nodiscard]] const auto& getFemaleNames() const { return femaleNames; }

  private:
	void registerkeys();

	std::set<std::string> dynastyNames;
	std::set<std::string> maleNames;
	std::set<std::string> femaleNames;
};
} // namespace mappers

#endif // NAME_LIST_ENTRY_H