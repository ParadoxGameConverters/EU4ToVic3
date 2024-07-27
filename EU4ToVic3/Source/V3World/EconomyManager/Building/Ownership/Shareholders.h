#ifndef V3_SHAREHOLDERS_H
#define V3_SHAREHOLDERS_H
#include <string>

namespace V3
{
struct Shareholders
{
	std::string type;
	std::string tag;
	std::string state;
	int level = 0;
};
} // namespace V3

#endif // V3_SHAREHOLDERS_H
