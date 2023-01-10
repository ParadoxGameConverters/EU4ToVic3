#ifndef VANILLA_TRUCE_ENTRY_H
#define VANILLA_TRUCE_ENTRY_H
#include "Parser.h"

namespace V3
{
class VanillaTruceEntry: commonItems::parser
{
  public:
	VanillaTruceEntry() = default;
	explicit VanillaTruceEntry(std::istream& theStream, std::string sourceTag);

	[[nodiscard]] const auto& getSourceTag() const { return sourceTag; }
	[[nodiscard]] const auto& getTargetTag() const { return targetTag; }
	[[nodiscard]] const auto& getDuration() const { return duration; }

  private:
	void registerKeys();

	std::string sourceTag;
	std::string targetTag;
	int duration = 0;
};
} // namespace V3
#endif // VANILLA_TRUCE_ENTRY_H