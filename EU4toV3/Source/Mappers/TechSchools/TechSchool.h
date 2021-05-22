#ifndef TECH_SCHOOL
#define TECH_SCHOOL

#include "Parser.h"

namespace mappers
{
	class TechSchool: commonItems::parser
	{
	public:
		explicit TechSchool(std::istream& theStream);

		[[nodiscard]] double calculateComparisonScore(
			double armyInvestment, 
			double commerceInvestment, 
			double cultureInvestment, 
			double industryInvestment, 
			double navyInvestment) const;

	private:
		double armyBonus = 0;
		double commerceBonus = 0;
		double cultureBonus = 0;
		double industryBonus = 0;
		double navyBonus = 0;
	};
}

#endif // TECH_SCHOOL