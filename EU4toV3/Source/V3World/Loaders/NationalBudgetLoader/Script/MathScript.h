#ifndef V3_MATH_SCRIPT_H
#define V3_MATH_SCRIPT_H
#include "Parser.h"

namespace V3
{
/* A Math script is a primitive form of Paradox's weight modifier script
 *  It only supports a single trigger.
 */
class Country;
class MathScript: commonItems::parser
{
  public:
	MathScript() = default;
	void loadMathScript(std::istream& theStream);

	[[nodiscard]] bool isValid(const Country& country) const;

  protected:
	double value = 0;

  private:
	void registerKeys();

	std::string trigger;		  // is_colony, vic3_law
	std::string triggerValue; // yes, law_serfdom
};
} // namespace V3

#endif // V3_MATH_SCRIPT_H