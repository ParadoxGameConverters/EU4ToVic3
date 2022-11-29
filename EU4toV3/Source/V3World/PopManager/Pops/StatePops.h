#ifndef STATE_POPS_H
#define STATE_POPS_H
#include "SubStatePops.h"
#include <optional>
#include <string>
#include <vector>

namespace V3
{
class StatePops
{
  public:
	StatePops() = default;

	void setSubStatePops(const std::vector<SubStatePops>& thePops) { subStatePops = thePops; }
	void setStateName(const std::string& theName) { stateName = theName; }

	[[nodiscard]] int getPopCount() const;
	[[nodiscard]] const auto& getSubStatePops() const { return subStatePops; }
	[[nodiscard]] const auto& getStateName() const { return stateName; }

	[[nodiscard]] std::optional<std::string> getDominantCulture() const;

  private:
	std::string stateName;
	std::vector<SubStatePops> subStatePops;
};
} // namespace V3
#endif // STATE_POPS_H