#ifndef ACCEPTED_CULTURE_THRESHOLDS_MAPPER_H
#define ACCEPTED_CULTURE_THRESHOLDS_MAPPER_H
#include "Parser.h"

namespace mappers
{
class AcceptedCultureThresholdsMapper: commonItems::parser
{
  public:
	AcceptedCultureThresholdsMapper();
	AcceptedCultureThresholdsMapper(std::istream& theStream);

	[[nodiscard]] auto getSameGroup() const { return sameGroup; }
	[[nodiscard]] auto getForeignGroup() const { return foreignGroup; }

  private:
	void registerKeys();
	double sameGroup = 0;
	double foreignGroup = 0;
};
} // namespace mappers

#endif // ACCEPTED_CULTURE_THRESHOLDS_MAPPER_H
