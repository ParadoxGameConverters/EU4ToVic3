#ifndef SUB_STATE_POPS_H
#define SUB_STATE_POPS_H
#include "Pop.h"
#include <optional>
#include <string>
#include <vector>

namespace V3
{
class SubStatePops
{
  public:
	SubStatePops() = default;

	void setPops(const std::vector<Pop>& thePops) { pops = thePops; }
	void addPop(const Pop& pop) { pops.push_back(pop); }
	void addPops(const std::vector<Pop>& thePops) { pops.insert(pops.end(), thePops.begin(), thePops.end()); }
	void setTag(const std::string& theTag) { tag = theTag; }
	void multiplyPops(double factor);

	[[nodiscard]] int getPopCount() const;
	[[nodiscard]] int getSlavePopCount() const;
	[[nodiscard]] const auto& getPops() const { return pops; }
	[[nodiscard]] const auto& getTag() const { return tag; }

	[[nodiscard]] std::optional<std::string> getDominantCulture() const;
	[[nodiscard]] std::optional<std::string> getDominantReligion() const;

  private:
	std::string tag;
	std::vector<Pop> pops;
};
} // namespace V3
#endif // SUB_STATE_POPS_H