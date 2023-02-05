#include "SkillMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include <numeric>

mappers::SkillMapping::SkillMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::SkillMapping::registerKeys()
{
	registerKeyword("sum", [this](std::istream& theStream) {
		sum = commonItems::getInt(theStream);
	});
	registerKeyword("prim", [this](std::istream& theStream) {
		primary = commonItems::getString(theStream);
	});
	registerKeyword("trait", [this](std::istream& theStream) {
		trait = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::SkillMapping::matchTrait(const std::vector<int>& stats) const
{
	const auto total = std::accumulate(stats.begin(), stats.end(), 0);
	if (total < sum)
		return std::nullopt;

	if (!primary)
		return trait;

	if ((*primary == "adm" || *primary == "dip" || *primary == "mil") && stats.size() == 3)
	{
		const auto adm = stats[0];
		const auto dip = stats[1];
		const auto mil = stats[2];

		if (*primary == "adm" && adm > dip && adm > mil)
			return trait;
		else if (*primary == "dip" && dip > adm && dip > mil)
			return trait;
		else if (*primary == "mil" && mil > adm && mil > dip)
			return trait;
	}
	else if ((*primary == "fir" || *primary == "sho" || *primary == "man" || *primary == "sie") && stats.size() == 4)
	{
		const auto fir = stats[0];
		const auto sho = stats[1];
		const auto man = stats[2];
		const auto sie = stats[3];

		if (*primary == "fir" && fir > sho && fir > man && fir > sie)
			return trait;
		else if (*primary == "sho" && sho > fir && sho > man && sho > sie)
			return trait;
		else if (*primary == "man" && man > fir && man > sho && man > sie)
			return trait;
		else if (*primary == "sie" && sie > fir && sie > sho && sie > man)
			return trait;
	}

	return std::nullopt;
}
