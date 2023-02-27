#ifndef STATE_MODIFIER_LOADER_H
#define STATE_MODIFIER_LOADER_H
#include "ClayManager/State/StateModifier.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class StateModifierLoader: commonItems::parser
{
  public:
	StateModifierLoader() = default;

	void loadStateModifiers(const commonItems::ModFilesystem& modFS);
	[[nodiscard]] const auto& getStateModifiers() const { return stateModifiers; }

  private:
	void registerKeys();

	std::map<std::string, StateModifier> stateModifiers;
};
} // namespace V3
#endif // STATE_MODIFIER_LOADER_H