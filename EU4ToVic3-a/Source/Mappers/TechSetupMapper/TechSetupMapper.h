#ifndef TECH_SETUP_MAPPER_H
#define TECH_SETUP_MAPPER_H
#include "Parser.h"
#include "TechSetupMapping.h"

namespace mappers
{
class TechSetupMapper: commonItems::parser
{
  public:
	TechSetupMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] std::set<std::string> getTechsForScoreTrack(const std::string& track, double score) const;

  private:
	void registerKeys();

	std::map<std::string, TechSetupMapping> trackTechScores; // track->techScores
};
} // namespace mappers

#endif // TECH_SETUP_MAPPER_H