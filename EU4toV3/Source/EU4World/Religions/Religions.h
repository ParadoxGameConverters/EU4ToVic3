#ifndef RELIGIONS_H
#define RELIGIONS_H
#include "Parser.h"
#include <set>

namespace EU4
{
class Religions: commonItems::parser
{
  public:
	Religions();
	explicit Religions(std::istream& theStream);

	[[nodiscard]] const auto& getAllReligions() const { return theReligions; }

  private:
	void registerKeys();

	std::set<std::string> theReligions;
};
} // namespace EU4

#endif // RELIGIONS_H