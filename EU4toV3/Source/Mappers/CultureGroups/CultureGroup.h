#ifndef CULTURE_GROUP_H
#define CULTURE_GROUP_H
#include "Parser.h"
#include <map>

namespace mappers
{
class Culture;
class CultureGroup: commonItems::parser
{
  public:
	CultureGroup() = default;
	CultureGroup(std::string theName, std::istream& theStream);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getHpm() const { return hpm; }
	[[nodiscard]] const auto& getCultures() const { return cultures; }
	[[nodiscard]] auto containsCulture(const std::string& cultureName) const { return cultures.contains(cultureName); }

	void addNeoCulture(const std::string& theName, const std::shared_ptr<Culture>& culture, const std::string& oldCulture);
	void mergeCulture(const std::string& theName, const std::shared_ptr<Culture>& culture);
	void clearUnionTag() { culturalUnionTag.clear(); }

	friend std::ostream& operator<<(std::ostream& output, const CultureGroup& cultureGroup);

  private:
	void registerKeys();

	std::string name;
	std::string hpm;
	std::string unit;
	std::string hpmUnit;
	std::string leader;
	std::string isOverseas;
	std::string culturalUnionTag;
	std::string hpmUnionTag;
	std::map<std::string, std::shared_ptr<Culture>> cultures;
};
} // namespace mappers

#endif // CULTURE_GROUP_H
