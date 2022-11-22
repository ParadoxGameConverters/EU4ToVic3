#ifndef STATE_H
#define STATE_H
#include "Province.h"
#include <Parser.h>
#include <string>

namespace V3
{
struct SubState;
class State: commonItems::parser
{
  public:
	State() = default;
	void loadState(std::istream& theStream);
	void setStateName(const std::string& theName) { name = theName; }
	void addSubState(const std::shared_ptr<SubState>& substate) { substates.push_back(substate); }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] bool containsProvince(const std::string& provinceName) const { return provinces.contains(provinceName); }
	[[nodiscard]] std::shared_ptr<Province> getProvince(const std::string& provinceName) const;
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] bool isCoastal() const { return coastal; }
	[[nodiscard]] const auto& getSubStates() const { return substates; }

	friend std::ostream& operator<<(std::ostream& output, const State& state);

  private:
	void registerKeys();
	void distributeLandshares();

	bool coastal = false;
	std::string name;
	std::map<std::string, std::shared_ptr<Province>> provinces; // in xA2345A format
	std::vector<std::shared_ptr<SubState>> substates;
};
} // namespace V3
#endif // STATE_H