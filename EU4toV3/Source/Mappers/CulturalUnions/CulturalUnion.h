#ifndef CULTURAL_UNION_H
#define CULTURAL_UNION_H
#include <set>
#include "Parser.h"

namespace mappers
{
class CulturalUnion: commonItems::parser
{
  public:
	explicit CulturalUnion(std::istream& theStream);

	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getTags() const { return tags; }

  private:
	void registerKeys();

	std::string culture;
	std::set<std::string> tags;
	std::set<std::string> hpm;
};
} // namespace mappers

#endif // CULTURAL_UNION_H
