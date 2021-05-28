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

	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	[[nodiscard]] auto getUpperRatio() const { return upperRatio; }
	[[nodiscard]] auto getMiddleRatio() const { return middleRatio; }
	[[nodiscard]] auto getLowerRatio() const { return lowerRatio; }

  private:
	std::string culture;
	std::string religion;
	double upperRatio = 1;
	double middleRatio = 1;
	double lowerRatio = 1;
};
} // namespace EU4

#endif // POP_RATIO_H