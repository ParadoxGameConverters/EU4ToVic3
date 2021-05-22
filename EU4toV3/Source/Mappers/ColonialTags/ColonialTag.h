#ifndef COLONIAL_TAG_H
#define COLONIAL_TAG_H
#include <set>
#include "Parser.h"

namespace mappers
{
typedef struct ColonyStruct
{
	[[nodiscard]] bool match(const std::string& eu4Region, const std::string& v2Region, const std::string& cultureGroup) const;
	std::string tag;
	std::set<std::string> EU4Regions;
	std::set<std::string> V2Regions;
	std::set<std::string> cultureGroups;
} ColonyStruct;

class ColonialTag: commonItems::parser
{
  public:
	explicit ColonialTag(std::istream& theStream);

	[[nodiscard]] const auto& getColonyTag() const { return colonyTag; }

  private:
	void registerKeys();

	ColonyStruct colonyTag;
};
} // namespace mappers

#endif // COLONIAL_TAG_H
