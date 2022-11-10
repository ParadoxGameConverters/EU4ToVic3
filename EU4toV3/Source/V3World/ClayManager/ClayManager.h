#ifndef CLAY_MANAGER_H
#define CLAY_MANAGER_H
#include "StateLoader/State.h"
#include <map>
#include <string>

namespace V3
{
class ClayManager
{
  public:
	ClayManager() = default;

	void initializeVanillaStates(const std::string& v3Path);

  private:
	std::map<std::string, std::shared_ptr<State>> states;
};
} // namespace V3
#endif // CLAY_MANAGER_H