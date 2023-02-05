#ifndef V3_MULTIPLY_SCRIPT_H
#define V3_MULTIPLY_SCRIPT_H
#include "MathScript.h"

namespace V3
{
class MultiplyScript: public MathScript
{
  public:
	MultiplyScript() = default;

	double operator*(double rhs) const;
	[[nodiscard]] static MultiplyScript combine(const std::vector<MultiplyScript>& multiplyVector);
};
} // namespace V3

#endif // V3_MULTIPLY_SCRIPT_H