#ifndef STATE_MODIFIER_LOADER_H
#define STATE_MODIFIER_LOADER_H
#include "ClayManager/State/StateModifier.h"
#include "Parser.h"

namespace V3
{
class StateModifierLoader: commonItems::parser
{
  public:
	StateModifierLoader() = default;

	void loadStateModifiers(const std::string& v3Path);
	[[nodiscard]] const auto& getStateModifiers() const { return stateModifiers; }

  private:
	void registerKeys();

	std::map<std::string, std::shared_ptr<StateModifier>> stateModifiers;
};
} // namespace V3
#endif // STATE_MODIFIER_LOADER_H