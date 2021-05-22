#ifndef POP_INSTANCE_H
#define POP_INSTANCE_H
#include "Parser.h"

namespace mappers
{
typedef struct PopDetails
{
	std::string popType;
	std::string culture;
	std::string religion;
	int size = 0;
} PopDetails;

class PopInstance: commonItems::parser
{

  public:
	PopInstance() = default;
	explicit PopInstance(std::istream& theStream, const std::string& thePopType);

	[[nodiscard]] auto getPopDetails() const { return popDetails; }

  private:
	void registerKeys();

	PopDetails popDetails;
};
} // namespace mappers

#endif // POP_INSTANCE_H
