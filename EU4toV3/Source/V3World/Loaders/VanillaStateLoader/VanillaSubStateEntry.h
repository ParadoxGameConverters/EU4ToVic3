#ifndef VANILLA_SUBSTATE_ENTRY_H
#define VANILLA_SUBSTATE_ENTRY_H
#include "Parser.h"

namespace V3
{
class VanillaSubStateEntry: commonItems::parser
{
  public:
	VanillaSubStateEntry() = default;
	explicit VanillaSubStateEntry(std::istream& theStream);

	[[nodiscard]] const auto& getOwnerTag() const { return ownerTag; }
	[[nodiscard]] const auto& getStateType() const { return stateType; }
	[[nodiscard]] const auto& getProvinces() const { return provinces; }

  private:
	void registerKeys();

	std::string ownerTag;
	std::string stateType;
	std::set<std::string> provinces;
};
} // namespace V3
#endif // VANILLA_SUBSTATE_ENTRY_H