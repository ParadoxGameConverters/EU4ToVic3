#ifndef RELIGION_H
#define RELIGION_H
#include "Parser.h"

namespace mappers
{
class Religion: commonItems::parser
{
  public:
	Religion() = default;
	explicit Religion(std::istream& theStream);

	[[nodiscard]] const auto& getHpm() const { return hpm; }

	friend std::ostream& operator<<(std::ostream& output, const Religion& religion);

  private:
	void registerKeys();

	std::string hpm;
	int icon;
	std::string colorString;
	std::string hpmColorString;
};
} // namespace mappers

#endif // RELIGION_H
