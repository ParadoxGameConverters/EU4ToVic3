#ifndef POP_RATIO_H
#define POP_RATIO_H
#include <string>

namespace EU4
{
class PopRatio
{
  public:
	PopRatio(std::string _culture, std::string _religion);
	bool operator==(const PopRatio& rhs) const;

	void decay(float diffInYears, double assimilationFactor);
	void increase(float diffInYears, double assimilationFactor);
	void convertFrom();
	void convertToCulture(const std::string& culture);
	void convertToReligion(const std::string& religion);
	void convertTo(const std::string& culture, const std::string& religion);
	void multiplyRatios(double factor);

	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	[[nodiscard]] auto getUpperRatio() const { return upperRatio; }
	[[nodiscard]] auto getMiddleRatio() const { return middleRatio; }
	[[nodiscard]] auto getLowerRatio() const { return lowerRatio; }

	void markSpent() { spent = true; }
	[[nodiscard]] auto isSpent() const { return spent; }
	void markNeoCulture() { neoCulture = true; }
	[[nodiscard]] auto isNeoCulture() const { return neoCulture; }

	void mergeRatio(const PopRatio& ratio);

  private:
	std::string culture;
	std::string religion;
	double upperRatio = 1;
	double middleRatio = 1;
	double lowerRatio = 1;
	bool spent = false;		 // Mark as disposable.
	bool neoCulture = false; // if true, V3World should map this to an appropriate neoculture.
};
} // namespace EU4

#endif // POP_RATIO_H