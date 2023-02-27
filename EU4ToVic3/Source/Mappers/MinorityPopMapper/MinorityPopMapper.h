#ifndef MINORITY_POP_MAPPER_H
#define MINORITY_POP_MAPPER_H
#include "Parser.h"
#include "PopManager/Pops/Pop.h"

namespace mappers
{
class MinorityPopMapper: commonItems::parser
{
  public:
	MinorityPopMapper() = default;
	explicit MinorityPopMapper(std::istream& theStream);
	void loadMappingRules(const std::string& filePath);

	[[nodiscard]] bool blankMajorityFromMinority(V3::Pop& pop) const;

  private:
	void registerKeys();

	std::vector<std::pair<std::string, std::string>> minorityPopMap;
};
}; // namespace mappers

#endif // MINORITY_POP_MAPPER_H
