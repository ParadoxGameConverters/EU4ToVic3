#ifndef IDEA_EFFECTS_MAPPING_H
#define IDEA_EFFECTS_MAPPING_H
#include "Parser.h"
#include <set>

namespace mappers
{
class IdeaEffectsMapping: commonItems::parser
{
  public:
	IdeaEffectsMapping() = default;
	explicit IdeaEffectsMapping(std::istream& theStream);

	[[nodiscard]] auto getLiteracy() const { return literacy; }
	[[nodiscard]] auto getAdm() const { return adm; }
	[[nodiscard]] auto getDip() const { return dip; }
	[[nodiscard]] auto getMil() const { return mil; }
	[[nodiscard]] const auto& getIgs() const { return igs; }
	[[nodiscard]] const auto& getNoIgs() const { return noIgs; }

  private:
	void registerKeys();

	int literacy = 0;
	int adm = 0;
	int dip = 0;
	int mil = 0;
	std::set<std::string> igs;
	std::set<std::string> noIgs;
};
} // namespace mappers

#endif // IDEA_EFFECTS_MAPPING_H