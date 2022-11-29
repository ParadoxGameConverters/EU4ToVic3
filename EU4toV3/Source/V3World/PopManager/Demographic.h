#ifndef V3_DEMOGRAPHIC_H
#define V3_DEMOGRAPHIC_H
#include <string>

namespace V3
{
struct Demographic
{
	std::string culture;
	std::string religion;
	double upperRatio = 0.0;
	double middleRatio = 0.0;
	double lowerRatio = 0.0;
};
} // namespace V3

#endif // V3_DEMOGRAPHIC_H
