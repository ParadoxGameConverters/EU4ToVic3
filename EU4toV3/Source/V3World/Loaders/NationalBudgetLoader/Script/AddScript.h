#ifndef V3_ADD_SCRIPT_H
#define V3_ADD_SCRIPT_H
#include "MathScript.h"

namespace V3
{
class AddScript: public MathScript
{
  public:
	AddScript() = default;

	double operator+(double rhs) const;
	[[nodiscard]] static AddScript combine(const std::vector<AddScript>& addVector);
};
} // namespace V3

#endif // V3_ADD_SCRIPT_H