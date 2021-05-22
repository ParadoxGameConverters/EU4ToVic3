#ifndef UNION_TAGS_MAPPER_H
#define UNION_TAGS_MAPPER_H
#include "Parser.h"
#include <set>

namespace mappers
{
class UnionTagsMapper: commonItems::parser
{
  public:
	UnionTagsMapper();
	explicit UnionTagsMapper(std::istream& theStream);

	[[nodiscard]] auto isTagUnionfull(const std::string& theTag) const { return unionTags.contains(theTag); }

  private:
	void registerKeys();

	std::set<std::string> unionTags;
};
} // namespace mappers

#endif // UnionTagsMapper
