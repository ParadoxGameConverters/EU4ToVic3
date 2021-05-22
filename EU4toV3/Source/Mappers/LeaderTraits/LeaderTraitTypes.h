#ifndef LEADER_TRAIT_TYPES_H
#define LEADER_TRAIT_TYPES_H
#include "LeaderTrait.h"
#include "Parser.h"

namespace mappers
{
class LeaderTraitTypes: commonItems::parser
{
  public:
	explicit LeaderTraitTypes(std::istream& theStream);

	[[nodiscard]] const auto& getTraits() const { return traits; }

  private:
	void registerKeys();

	std::vector<LeaderTrait> traits;
};
} // namespace mappers

#endif // LEADER_TRAIT_TYPES_H