#include "PopRatio.h"
#include <algorithm>
#include <cmath>

EU4::PopRatio::PopRatio(std::string _culture, std::string _religion): culture(std::move(_culture)), religion(std::move(_religion))
{
}

void EU4::PopRatio::decay(const float diffInYears, const double assimilationFactor)
{
	// for standard factor 0.0025 ratio will decrease by 0.25% every year. It will never hit 0%
	// for any assimilation factor but will progressively decrease at an ever-slower rate.

	upperRatio -= upperRatio * (1 - pow(1 - assimilationFactor, diffInYears));
	middleRatio -= middleRatio * (1 - pow(1 - assimilationFactor, diffInYears));
	lowerRatio -= lowerRatio * (1 - pow(1 - assimilationFactor, diffInYears));
}

void EU4::PopRatio::increase(const float diffInYears, const double assimilationFactor)
{
	// for standard factor 0.0025 ratio will increase by 0.25% of the remainder every year. It will never hit 100%
	// for any assimilation factor but will progressively increase at an ever-slower rate.

	upperRatio += (1 - upperRatio) * (1 - pow(1 - assimilationFactor, diffInYears));
	middleRatio += (1 - middleRatio) * (1 - pow(1 - assimilationFactor, diffInYears));
	lowerRatio += (1 - lowerRatio) * (1 - pow(1 - assimilationFactor, diffInYears));
}

void EU4::PopRatio::convertFrom()
{
	upperRatio /= 2;
	middleRatio /= 2;
}

void EU4::PopRatio::convertToCulture(const std::string& _culture)
{
	upperRatio = 0.5;
	middleRatio = 0.5;
	lowerRatio = 0;
	culture = _culture;
}

void EU4::PopRatio::convertToReligion(const std::string& _religion)
{
	upperRatio = 0.5;
	middleRatio = 0.5;
	lowerRatio = 0;
	religion = _religion;
}

void EU4::PopRatio::convertTo(const std::string& _culture, const std::string& _religion)
{
	upperRatio = 0.5;
	middleRatio = 0.5;
	lowerRatio = 0;
	culture = _culture;
	religion = _religion;
}

void EU4::PopRatio::setNeoCulture(const std::string& neoCulture, const std::string& superRegion)
{
	// Used to override history entries when loading neocultures.

	if (originalEU4culture.empty())
		originalEU4culture = culture;
	culture = neoCulture;
	targetSuperRegion = superRegion;
}
