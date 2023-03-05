#ifndef LAW_MAPPER_H
#define LAW_MAPPER_H
#include "LawMapping.h"
#include "Loaders/LawLoader/Law.h"
#include "Parser.h"

namespace V3
{
class Country;
}
namespace mappers
{
typedef std::map<std::string, LawMapping> Laws;
class LawMapper: commonItems::parser
{
  public:
	LawMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadLawDefinitions(const std::map<std::string, V3::Law>& theLaws) { laws = theLaws; }

	[[nodiscard]] const auto& getLawGroups() const { return lawGroups; }
	[[nodiscard]] const auto& getLaws() const { return laws; }

	[[nodiscard]] std::optional<std::string> grantLawFromGroup(const std::string& lawGroup, const V3::Country& country) const;

  private:
	void registerKeys();

	[[nodiscard]] bool isLawBlocked(const std::string& lawName, const std::set<std::string>& existingLaws, const std::set<std::string>& existingTechs) const;
	[[nodiscard]] std::vector<std::string> filterLawsByAllowedProgressiveness(const std::set<std::string>& nonBlockedLaws) const;

	std::map<std::string, Laws> lawGroups;
	std::map<std::string, V3::Law> laws;
};
} // namespace mappers

#endif // LAW_MAPPER_H