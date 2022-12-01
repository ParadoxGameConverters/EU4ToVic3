#ifndef STATE_LOADER_H
#define STATE_LOADER_H
#include "Parser.h"

namespace V3
{
class State;
class StateLoader: commonItems::parser
{
  public:
	StateLoader() = default;

	void loadStates(const std::string& v3Path);
	[[nodiscard]] const auto& getStates() const { return states; }

  private:
	void registerKeys();

	std::map<std::string, std::shared_ptr<State>> states;
};
} // namespace V3
#endif // STATE_LOADER_H