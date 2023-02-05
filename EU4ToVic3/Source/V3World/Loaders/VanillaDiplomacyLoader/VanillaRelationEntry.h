#ifndef VANILLA_RELATION_ENTRY_H
#define VANILLA_RELATION_ENTRY_H
#include "Parser.h"

namespace V3
{
class VanillaRelationEntry: commonItems::parser
{
  public:
	VanillaRelationEntry() = default;
	explicit VanillaRelationEntry(std::istream& theStream, std::string sourceTag);

	[[nodiscard]] const auto& getSourceTag() const { return sourceTag; }
	[[nodiscard]] const auto& getTargetTag() const { return targetTag; }
	[[nodiscard]] const auto& getRelationValue() const { return relationValue; }

  private:
	void registerKeys();

	std::string sourceTag;
	std::string targetTag;
	int relationValue = 0;
};
} // namespace V3
#endif // VANILLA_RELATION_ENTRY_H