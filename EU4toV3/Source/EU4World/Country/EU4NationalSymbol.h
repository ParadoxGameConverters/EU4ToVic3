#ifndef EU4_NATIONAL_SYMBOL_H
#define EU4_NATIONAL_SYMBOL_H
#include "EU4CustomColors.h"

namespace EU4
{
class NationalSymbol: public commonItems::parser
{
  public:
	NationalSymbol() = default;
	explicit NationalSymbol(std::istream& theStream);

	[[nodiscard]] const auto& getMapColor() const { return mapColor; }
	[[nodiscard]] const auto& getRevolutionaryColor() const { return revolutionaryColor; }
	[[nodiscard]] const auto& getCustomColors() const { return customColors; }

	void setMapColor(const commonItems::Color& color) { mapColor = color; }
	void setRevolutionaryColor(const commonItems::Color& color) { revolutionaryColor = color; }
	void setCustomColors(const std::shared_ptr<CustomColors>& colors) { customColors = colors; }

  private:
	void registerKeys();

	std::shared_ptr<CustomColors> customColors;
	std::optional<commonItems::Color> mapColor;
	std::optional<commonItems::Color> revolutionaryColor;
};
} // namespace EU4

#endif // EU4_NATIONAL_SYMBOL_H
