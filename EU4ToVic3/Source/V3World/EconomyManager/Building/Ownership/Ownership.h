#ifndef V3_OWNERSHIP_H
#define V3_OWNERSHIP_H
#include <string>

namespace V3
{
struct Ownership
{
	std::string type;
	double weight = 0.0;
	double colonialFrac = 0.0;
	double financialCenterFrac = 0.0;
};
} // namespace V3

#endif // V3_OWNERSHIP_H
