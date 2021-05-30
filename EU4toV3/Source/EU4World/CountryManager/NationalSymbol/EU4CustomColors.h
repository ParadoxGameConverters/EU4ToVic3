#ifndef EU4_CUSTOM_COLORS_H
#define EU4_CUSTOM_COLORS_H
#include "Color.h"
#include "EU4CustomColorsBlock.h"

namespace EU4
{
class CustomColors: public commonItems::parser
{
  public:
	CustomColors() = default;
	explicit CustomColors(std::istream& theStream);

	[[nodiscard]] const auto& getCustomColorsBlock() const { return customColors; }

  private:
	void registerKeys();

	CustomColorsBlock customColors; // It walks like a color, it quacks like a color, but it's in fact a color INDEX from 00_custom_colors.txt
};
} // namespace EU4

#endif // EU4_CUSTOM_COLORS_H
