#ifndef V3_BUY_PACKAGE_H
#define V3_BUY_PACKAGE_H
#include "Parser.h"

namespace V3
{
class BuyPackage: commonItems::parser
{
  public:
	BuyPackage() = default;
	explicit BuyPackage(std::istream& theStream);

	void setWealth(const int theWealth) { wealth = theWealth; }

	[[nodiscard]] const auto& getWealth() const { return wealth; }
	[[nodiscard]] const auto& getPopneeds() const { return popneedMap; }

  private:
	void registerKeys();
	parser goodsParser;

	int wealth = 0;
	std::map<std::string, int> popneedMap; // popneed to magnitude
};
} // namespace V3

#endif // V3_BUY_PACKAGE_H