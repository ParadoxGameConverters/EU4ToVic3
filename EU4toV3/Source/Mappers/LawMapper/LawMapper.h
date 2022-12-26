#ifndef LAW_MAPPER_H
#define LAW_MAPPER_H
#include "LawMapping.h"
#include "Loaders/LawLoader/Law.h"
#include "Parser.h"

namespace mappers
{
typedef std::map<std::string, LawMapping> Laws;
class LawMapper: commonItems::parser
{
  public:
	LawMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);
	void loadLawDefinitions(const std::map<std::string, V3::Law>& theLaws) { laws = theLaws; }

	[[nodiscard]] const auto& getLawGroups() const { return lawGroups; }

  private:
	void registerKeys();

	std::map<std::string, Laws> lawGroups;
	std::map<std::string, V3::Law> laws;
};
} // namespace mappers

#endif // LAW_MAPPER_H