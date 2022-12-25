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
	std::map<std::string, int> igCounter;
	for (const auto& idea: ideas)
	{
		if (!ideaEffects.contains(idea))
			continue; // No need to scream for unmapped mod ideas
		const auto ideaEffect = ideaEffects.at(idea);
		effect.literacy += ideaEffect.getLiteracy();
		effect.adm += ideaEffect.getAdm();
		effect.dip += ideaEffect.getDip();
		effect.mil += ideaEffect.getMil();
		for (const auto& ig: ideaEffect.getIgs())
		{
			if (igCounter.contains(ig))
				++igCounter.at(ig);
			else
				igCounter.emplace(ig, 1);
		}
		for (const auto& noIg: ideaEffect.getNoIgs())
		{
			if (igCounter.contains(noIg))
				--igCounter.at(noIg);
			else
				igCounter.emplace(noIg, -1);
		}
	}
	for (const auto& [ig, count]: igCounter)
		if (count > 0)
			effect.igs.emplace(ig);
		else if (count < 0)
			effect.noIgs.emplace(ig);

	return effect;
}
