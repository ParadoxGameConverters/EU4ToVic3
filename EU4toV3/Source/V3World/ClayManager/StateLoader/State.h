#ifndef STATE_H
#define STATE_H
#include "Province.h"
#include <Parser.h>
#include <string>

namespace V3
{
class State: commonItems::parser
{
  public:
	State() = default;
	void loadState(std::istream& theStream);
	void setStateName(const std::string theName) { name = theName; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] bool containsProvince(const std::string& provinceName) const { return provinces.contains(provinceName); }
	[[nodiscard]] const auto& getProvince(const std::string& provinceName) const { return provinces.at(provinceName); }
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] bool isCoastal() const { return coastal; }

  private:
	void registerKeys();

	bool coastal = false;
	std::string name;
	std::map<std::string, std::shared_ptr<Province>> provinces; // in xA2345A format
};
} // namespace V3
#endif // STATE_H