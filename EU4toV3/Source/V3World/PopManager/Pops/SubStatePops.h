#ifndef SUB_STATE_POPS_H
#define SUB_STATE_POPS_H
#include "Pop.h"
#include <string>
#include <vector>

namespace V3
{
class SubStatePops
{
  public:
	SubStatePops() = default;

	void setPops(const std::vector<Pop>& thePops) { pops = thePops; }
	void setTag(const std::string& theTag) { tag = theTag; }

	[[nodiscard]] int getPopCount() const;
	[[nodiscard]] const auto& getPops() const { return pops; }
	[[nodiscard]] const auto& getTag() const { return tag; }

  private:
	std::string tag;
	std::vector<Pop> pops;
};
} // namespace V3
#endif // SUB_STATE_POPS_H