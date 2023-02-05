#ifndef EU4_CUSTOM_COLORS_BLOCK
#define EU4_CUSTOM_COLORS_BLOCK
#include "Color.h"

namespace EU4
{
struct CustomColorsBlock
{
	int flagIndex = 0;
	int colorIndex = 0;
	int symbolIndex = 0;
	std::optional<commonItems::Color> flagColors;
};
} // namespace EU4

#endif // EU4_CUSTOM_COLORS_BLOCK
