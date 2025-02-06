#include "IdeaEffectsMapper.h"
#include "CommonRegexes.h"
#include "Log.h"

void mappers::IdeaEffectsMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading Idea Effects.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << ideaEffects.size() << " effects loaded.";
}

void mappers::IdeaEffectsMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::IdeaEffectsMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& ideaName, std::istream& theStream) {
		ideaEffects.emplace(ideaName, IdeaEffectsMapping(theStream));
	});
}

mappers::IdeaEffect mappers::IdeaEffectsMapper::getEffectForIdeas(const std::set<std::string>& ideas) const
{
	IdeaEffect effect;
	std::map<std::string, int> interestGroupCounter;
	for (const auto& idea: ideas)
	{
		if (!ideaEffects.contains(idea))
			continue; // No need to scream for unmapped mod ideas
		const auto ideaEffect = ideaEffects.at(idea);
		effect.literacy += ideaEffect.getLiteracy();
		effect.adm += ideaEffect.getAdm();
		effect.dip += ideaEffect.getDip();
		effect.mil += ideaEffect.getMil();
		for (const auto& ig: ideaEffect.getBoostedInterestGroups())
		{
			if (interestGroupCounter.contains(ig))
				++interestGroupCounter.at(ig);
			else
				interestGroupCounter.emplace(ig, 1);
		}
		for (const auto& noIg: ideaEffect.getSuppressedInterestGroups())
		{
			if (interestGroupCounter.contains(noIg))
				--interestGroupCounter.at(noIg);
			else
				interestGroupCounter.emplace(noIg, -1);
		}
	}
	for (const auto& [ig, count]: interestGroupCounter)
		if (count > 0)
		{
			// Temporarily disabling boosting and suppression until it becomes clear what's going on with these. Both seems to have been disabled/deleted on 1.8+.
			// effect.boostedInterestGroups.emplace(ig);
			if (count >= 2)
				effect.rulingInterestGroups.emplace(ig);
		}
		else if (count < 0)
		{
			// effect.suppressedInterestGroups.emplace(ig);
		}

	return effect;
}
