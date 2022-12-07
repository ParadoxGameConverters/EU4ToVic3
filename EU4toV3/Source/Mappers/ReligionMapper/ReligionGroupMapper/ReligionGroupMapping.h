#ifndef RELIGION_GROUP_MAPPING_H
#define RELIGION_GROUP_MAPPING_H
#include "Parser.h"

namespace mappers
{
class ReligionGroupMapping: commonItems::parser
{
  public:
	explicit ReligionGroupMapping(std::istream& theStream);

	[[nodiscard]] const auto& getTrait() const { return trait; }
	[[nodiscard]] const auto& getEU4Groups() const { return eu4groups; }
	[[nodiscard]] const auto& getTaboos() const { return taboos; }
	[[nodiscard]] const auto& getIcon() const { return icon; }

  private:
	void registerkeys();

	std::string trait;
	std::set<std::string> eu4groups;
	std::set<std::string> taboos;
	std::string icon;
};
} // namespace mappers

#endif // RELIGION_GROUP_MAPPING_H