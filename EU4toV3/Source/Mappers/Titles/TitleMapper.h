#ifndef TITLE_MAPPER_H
#define TITLE_MAPPER_H
#include "Parser.h"
#include <map>
#include <optional>
#include <set>

namespace mappers
{
class TitleMapper: commonItems::parser
{
  public:
	TitleMapper();
	explicit TitleMapper(std::istream& theStream);

	[[nodiscard]] bool doesTitleExist(const std::string& title) const;
	[[nodiscard]] std::optional<std::string> getTitleForName(const std::string& name) const;
	[[nodiscard]] std::optional<std::string> getRandomIslamicTitle() const;
	[[nodiscard]] std::optional<std::string> getRandomIndianTitle() const;

  private:
	void registerKeys();

	std::map<std::string, std::string> titleMap; // <name, title>
	std::set<std::string> titles;
	std::vector<std::string> islamicTitles;
	std::vector<std::string> indianTitles;
};
} // namespace mappers

#endif // CK2_TITLE_MAPPER_H