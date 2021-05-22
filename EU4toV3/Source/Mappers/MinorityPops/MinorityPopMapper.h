#ifndef MINORITY_POP_MAPPER_H
#define MINORITY_POP_MAPPER_H
#include "Parser.h"
#include "Pop/Pop.h"

namespace mappers
{
class MinorityPopMapper: commonItems::parser
{
  public:
	MinorityPopMapper();
	explicit MinorityPopMapper(std::istream& theStream);

	[[nodiscard]] bool blankMajorityFromMinority(V2::Pop& pop) const;

  private:
	void registerKeys();

	std::vector<std::pair<std::string, std::string>> minorityPopMap;
};
}; // namespace mappers

#endif // MINORITY_POP_MAPPER_H
