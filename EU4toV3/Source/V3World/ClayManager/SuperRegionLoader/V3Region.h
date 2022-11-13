#ifndef V3_REGION_H
#define V3_REGION_H
#include "../StateLoader/State.h"
#include "Parser.h"

namespace V3
{
class Region: commonItems::parser
{
  public:
	Region() = default;

	void initializeRegion(std::istream& theStream);
	void setName(const std::string& theName) { name = theName; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getStates() const { return states; }
	[[nodiscard]] bool containsState(const std::string& state) const { return states.contains(state); }
	void replaceStates(const std::map<std::string, std::shared_ptr<State>>& theStates) { states = theStates; }

  private:
	void registerKeys();

	std::string name;
	std::map<std::string, std::shared_ptr<State>> states;
};
} // namespace V3
#endif // V3_REGION_H