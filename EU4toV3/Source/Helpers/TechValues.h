#ifndef TECH_CONVERSION_HELPERS_H
#define TECH_CONVERSION_HELPERS_H

#include <map>
#include <string>
#include <memory>
#include <vector>

namespace EU4
{
	class Country;
}
namespace mappers
{
	class IdeaEffectMapper;
	class TechgroupsMapper;
}
namespace V2
{
	class Country;
}

namespace helpers
{
	class TechValues
	{
	public:
		explicit TechValues(const std::map<std::string, std::shared_ptr<V2::Country>>& countries);

		[[nodiscard]] static bool isValidCountryForTechConversion(const V2::Country& country);
		[[nodiscard]] double getNormalizedArmyTech(const EU4::Country& country) const;
		[[nodiscard]] double getNormalizedNavyTech(const EU4::Country& country) const;
		[[nodiscard]] double getNormalizedCommerceTech(const EU4::Country& country) const;
		[[nodiscard]] double getNormalizedCultureTech(const EU4::Country& country) const;
		[[nodiscard]] double getNormalizedIndustryTech(const EU4::Country& country) const;

	private:
		[[nodiscard]] static double getCountryArmyTech(const EU4::Country& country);
		[[nodiscard]] static double getCountryNavyTech(const EU4::Country& country);
		[[nodiscard]] static double getCountryCommerceTech(const EU4::Country& country);
		[[nodiscard]] static double getCountryCultureTech(const EU4::Country& country);
		[[nodiscard]] static double getCountryIndustryTech(const EU4::Country& country);

		void gatherScores(const std::map<std::string, std::shared_ptr<V2::Country>>& countries);
		void calculateSteps();

		double armyStep = 0;
		double navyStep = 0;
		double commerceStep = 0;
		double cultureStep = 0;
		double industryStep = 0;
		std::vector<double> armyScores;
		std::vector<double> navyScores;
		std::vector<double> commerceScores;
		std::vector<double> cultureScores;
		std::vector<double> industryScores;

	};
}

#endif // TECH_CONVERSION_HELPERS_H