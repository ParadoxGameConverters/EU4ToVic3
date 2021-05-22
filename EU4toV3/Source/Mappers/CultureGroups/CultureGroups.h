#ifndef CULTURE_GROUPS_H
#define CULTURE_GROUPS_H
#include "Parser.h"

namespace EU4
{
class Regions;
}

namespace mappers
{
class CultureGroup;
class CultureMapper;
class CultureGroups: commonItems::parser
{
  public:
	CultureGroups() = default;
	explicit CultureGroups(std::istream& theStream);

	void initForEU4();
	void initForV2();
	void importNeoCultures(const EU4::Regions& regions, const std::shared_ptr<CultureGroups>& eu4CultureGroupsMapper, const CultureMapper& cultureMapper) const;

	[[nodiscard]] std::shared_ptr<CultureGroup> getGroupForCulture(const std::string& cultureName) const;
	[[nodiscard]] const auto& getCultureGroupsMap() const { return cultureGroupsMap; }

	friend std::ostream& operator<<(std::ostream& output, const CultureGroups& cultureGroupsMapper);

  private:
	void registerKeys();

	std::map<std::string, std::shared_ptr<CultureGroup>> cultureGroupsMap;
};
} // namespace mappers

#endif // CULTURE_GROUPS_H
