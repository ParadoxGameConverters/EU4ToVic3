#include "LawMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::LawMapping::LawMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::LawMapping::registerKeys()
{
	registerKeyword("forms", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			forms.emplace(entry);
	});
	registerKeyword("ideas", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			ideas.emplace(entry);
	});
	registerKeyword("reforms", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			reforms.emplace(entry);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

double mappers::LawMapping::getScore(const std::string& incForm, const std::set<std::string>& incIdeas, const std::set<std::string>& incReforms) const
{
	double score = 0;
	if (forms.contains(incForm))
		score += 0.01; // we're just tagging this choice as fallback.
	for (const auto& incIdea: incIdeas)
		if (ideas.contains(incIdea))
			++score;
	for (const auto& incReform: incReforms)
		if (reforms.contains(incReform))
			++score;
	return score;
}
