#ifndef V3_REGION_H
#define V3_REGION_H
#include "Parser.h"

namespace V3
{
class State;
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

	[[nodiscard]] double getTotalSubStateWeight() const;

  private:
	void registerKeys();

	std::string name;
	std::map<std::string, std::shared_ptr<State>> states;
};
} // namespace V3
#endif // V3_REGION_H