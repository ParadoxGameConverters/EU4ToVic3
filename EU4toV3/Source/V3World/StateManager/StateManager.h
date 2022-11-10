#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H
#include <map>
#include <memory>
#include <string>

namespace V3
{
class StateManager
{
  public:
	StateManager() = default;
	void loadStates(const std::string& v3Path);

  private:
	std::map<std::string, std::shared_ptr<State>> states;
};

} // namespace V3

#endif // STATE_MANAGER_H