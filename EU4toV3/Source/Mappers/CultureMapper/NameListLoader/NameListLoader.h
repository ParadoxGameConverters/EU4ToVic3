#ifndef NAME_LIST_LOADER_H
#define NAME_LIST_LOADER_H
#include "NameListEntry.h"
#include "Parser.h"

namespace mappers
{
class NameListLoader: commonItems::parser
{
  public:
	NameListLoader() = default;
	void loadNameLists(const std::string& nameListPath);
	void loadNameLists(std::istream& theStream);

	[[nodiscard]] std::optional<NameListEntry> getNameList(const std::string& nameListName) const;

  private:
	void registerKeys();

	std::map<std::string, NameListEntry> nameLists;
};
} // namespace mappers

#endif // NAME_LIST_LOADER_H
