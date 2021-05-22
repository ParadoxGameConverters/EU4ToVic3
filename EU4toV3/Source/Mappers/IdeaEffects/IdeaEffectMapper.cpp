#include "IdeaEffectMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::IdeaEffectMapper::IdeaEffectMapper()
{
	LOG(LogLevel::Info) << "Parsing idea effects";
	registerKeys();
	parseFile("configurables/idea_effects.txt");
	parseFile("configurables/reform_effects.txt");
	clearRegisteredKeywords();
}

mappers::IdeaEffectMapper::IdeaEffectMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::IdeaEffectMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& idea, std::istream& theStream) {
		const IdeaEffects ideaEffects(theStream);
		ideas.insert(std::pair(idea, ideaEffects));
	});
}

std::optional<std::string> mappers::IdeaEffectMapper::getEnforceFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getEnforce();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getArmyFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getArmy();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getCommerceFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getCommerce();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getCultureFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getCulture();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getIndustryFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getIndustry();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getNavyFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getNavy();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getLiberalFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getLiberal();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getReactionaryFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getReactionary();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getLiteracyFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getLiteracy();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getSlaveryFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getSlavery();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getUpper_house_compositionFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getUpper_house_composition();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getVote_franchiseFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getVote_franchise();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getVoting_systemFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getVoting_system();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getPublic_meetingsFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getPublic_meetings();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getPress_rightsFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getPress_rights();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getTrade_unionsFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getTrade_unions();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getPolitical_partiesFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getPolitical_parties();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getOrderFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getOrder();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getLibertyFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getLiberty();
	return std::nullopt;
}

std::optional<int> mappers::IdeaEffectMapper::getEqualityFromIdea(const std::string& ideaName) const
{
	const auto& idea = ideas.find(ideaName);
	if (idea != ideas.end())
		return idea->second.getEquality();
	return std::nullopt;
}