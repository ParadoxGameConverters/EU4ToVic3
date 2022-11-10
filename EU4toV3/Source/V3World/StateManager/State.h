#ifndef STATE_H
#define STATE_H
#include <map>
#include <memory>
#include <string>

namespace V3
{
class State
{
  public:
	State() = default;

  private:
	std::map<std::string, std::shared_ptr<State>> states;
};

} // namespace V3

#endif // STATE_H