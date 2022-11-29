#ifndef POP_MANAGER_H
#define POP_MANAGER_H
#include "Pops/StatePops.h"
#include <map>
#include <string>

namespace V3
{
class ClayManager;
class PopManager
{
  public:
	PopManager() = default;

	void initializeVanillaPops(const std::string& v3Path);
	void assignVanillaPopsToStates(const ClayManager& clayManager);
	static void importDemographics(const ClayManager& clayManager);

  private:
	std::map<std::string, StatePops> vanillaStatePops; // state, StatePops
};
} // namespace V3
#endif // POP_MANAGER_H