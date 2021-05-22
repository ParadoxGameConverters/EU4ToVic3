#ifndef RELIGIOUS_GROUPS_H
#define RELIGIOUS_GROUPS_H
#include "Parser.h"

namespace mappers
{
class ReligiousGroup;
class ReligiousGroups: commonItems::parser
{
  public:
	ReligiousGroups();
	explicit ReligiousGroups(std::istream& theStream);

	[[nodiscard]] const auto& getReligiousGroupsMap() const { return religiousGroupsMap; }

	friend std::ostream& operator<<(std::ostream& output, const ReligiousGroups& religiousGroupsMapper);

  private:
	void registerKeys();

	std::map<std::string, std::shared_ptr<ReligiousGroup>> religiousGroupsMap;
};
} // namespace mappers

#endif // RELIGIOUS_GROUPS_H
