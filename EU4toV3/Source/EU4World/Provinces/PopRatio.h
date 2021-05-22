#ifndef POP_RATIO_H
#define POP_RATIO_H
#include <string>

namespace EU4
{
class PopRatio
{
  public:
	PopRatio(std::string _culture, std::string _religion);

	void decay(float diffInYears, double assimilationFactor);
	void increase(float diffInYears, double assimilationFactor);
	void convertFrom();
	void convertToCulture(const std::string& culture);
	void convertToReligion(const std::string& religion);
	void convertTo(const std::string& culture, const std::string& religion);
	void setNeoCulture(const std::string& neoCulture, const std::string& superRegion);

	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getOriginalCulture() const { return originalEU4culture; }
	[[nodiscard]] const auto& getSuperRegion() const { return targetSuperRegion; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	[[nodiscard]] auto getUpperRatio() const { return upperRatio; }
	[[nodiscard]] auto getMiddleRatio() const { return middleRatio; }
	[[nodiscard]] auto getLowerRatio() const { return lowerRatio; }

  private:
	std::string culture;
	std::string originalEU4culture; // Will be blank unless culture was swapped for a neoculture.
	std::string targetSuperRegion;  // for neoculture, this is our target superregion
	std::string religion;
	double upperRatio = 1;
	double middleRatio = 1;
	double lowerRatio = 1;
};
} // namespace EU4

#endif // POP_RATIO_H