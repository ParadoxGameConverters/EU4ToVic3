#ifndef CULTURAL_UNION_MAPPER_H
#define CULTURAL_UNION_MAPPER_H
#include <set>
#include "Parser.h"

namespace mappers
{
class CulturalUnionMapper: commonItems::parser
{
  public:
	CulturalUnionMapper(const std::string& filename);
	explicit CulturalUnionMapper(std::istream& theStream);

	[[nodiscard]] std::set<std::string> getCoresForCulture(const std::string& culture) const;
	[[nodiscard]] std::set<std::string> getCulturesForCore(const std::string& core) const;

  private:
	void registerKeys();
	std::map<std::string, std::set<std::string>> unionMap;
};
} // namespace mappers

#endif // CULTURAL_UNION_MAPPER_H