#ifndef COLONIAL_TAG_MAPPER_H
#define COLONIAL_TAG_MAPPER_H
#include "ColonialTag.h"

namespace mappers
{
class ColonialTagMapper: commonItems::parser
{
  public:
	ColonialTagMapper();
	explicit ColonialTagMapper(std::istream& theStream);

	std::optional<std::string> getColonialTag(const std::string& eu4Region, const std::string& v2Region, const std::string& cultureGroup) const;
	
  private:
	void registerKeys();

	std::vector<ColonyStruct> colonyList;
};
} // namespace mappers

#endif // COLONIAL_TAG_MAPPER_H
