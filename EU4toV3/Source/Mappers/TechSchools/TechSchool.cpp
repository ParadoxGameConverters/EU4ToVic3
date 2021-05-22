#include "TechSchool.h"
#include "ParserHelpers.h"
#include <cmath>

mappers::TechSchool::TechSchool(std::istream& theStream)
{
	registerKeyword("army_tech_research_bonus", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleDouble bonusDbl(theStream);
			armyBonus = bonusDbl.getDouble();
		});
	registerKeyword("commerce_tech_research_bonus", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleDouble bonusDbl(theStream);
			commerceBonus = bonusDbl.getDouble();
		});
	registerKeyword("culture_tech_research_bonus", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleDouble bonusDbl(theStream);
			cultureBonus = bonusDbl.getDouble();
		});
	registerKeyword("industry_tech_research_bonus", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleDouble bonusDbl(theStream);
			industryBonus = bonusDbl.getDouble();
		});
	registerKeyword("navy_tech_research_bonus", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleDouble bonusDbl(theStream);
			navyBonus = bonusDbl.getDouble();
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}

double mappers::TechSchool::calculateComparisonScore(
	const double armyInvestment,
	const double commerceInvestment,
	const double cultureInvestment,
	const double industryInvestment,
	const double navyInvestment) const
{
	return std::abs(armyInvestment - armyBonus - 0.2) +
		std::abs(navyInvestment - navyBonus - 0.2) +
		std::abs(commerceInvestment - commerceBonus - 0.2) +
		std::abs(industryInvestment - industryBonus - 0.2) +
		std::abs(cultureInvestment - cultureBonus - 0.2);
}
