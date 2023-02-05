#ifndef CHARACTER_TRAIT_MAPPER_H
#define CHARACTER_TRAIT_MAPPER_H
#include "Parser.h"
#include "SkillMapping.h"

namespace EU4
{
struct Character;
}
namespace mappers
{
class CharacterTraitMapper: commonItems::parser
{
  public:
	CharacterTraitMapper() = default;
	void loadMappingRules(const std::string& filePath);

	[[nodiscard]] std::optional<std::string> getPersonality(const std::string& eu4Personality) const;
	[[nodiscard]] std::set<std::string> getSkillTraits(const EU4::Character& eu4Character) const;
	[[nodiscard]] std::string getGratisIncompetency(int seed) const;
	[[nodiscard]] std::string getGratisVeterancy(int seed) const;
	[[nodiscard]] std::string getGratisAgeism(int seed) const;
	[[nodiscard]] std::string getGratisDisorder(int seed) const;

  private:
	void registerKeys();
	[[nodiscard]] std::optional<std::string> matchLeaderSkill(const std::string& leaderType, const std::vector<int>& skills) const;


	std::map<std::string, std::string> personalities;
	std::map<std::string, std::string> global;
	std::vector<SkillMapping> admirals;
	std::vector<SkillMapping> generals;
	std::vector<SkillMapping> rulers;
	std::set<std::string> incompetencies;
	std::set<std::string> veterancies;
	std::set<std::string> ageisms;
	std::set<std::string> disorders;
};
} // namespace mappers

#endif // CHARACTER_TRAIT_MAPPER_H