#ifndef IDEA_EFFECTS_MAPPER_H
#define IDEA_EFFECTS_MAPPER_H
#include "IdeaEffectsMapping.h"
#include "Parser.h"

namespace mappers
{
class IdeaEffectsMapper: commonItems::parser
{
  public:
	struct Effect
	{
		int literacy = 0;
		int adm = 0;
		int dip = 0;
		int mil = 0;
		std::set<std::string> igs;
		std::set<std::string> noIgs;
	};

	IdeaEffectsMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] Effect getEffectForIdeas(const std::set<std::string>& ideas) const;

  private:
	void registerKeys();

	std::map<std::string, IdeaEffectsMapping> ideaEffects; // idea->effect
};
} // namespace mappers

#endif // IDEA_EFFECTS_MAPPER_H