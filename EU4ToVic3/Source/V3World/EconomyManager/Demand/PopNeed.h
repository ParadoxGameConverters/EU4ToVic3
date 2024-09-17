#ifndef V3_POP_NEED_H
#define V3_POP_NEED_H
#include "GoodsFulfillment.h"
#include "Parser.h"

namespace V3
{
class PopNeed: commonItems::parser
{
  public:
	PopNeed() = default;
	explicit PopNeed(std::istream& theStream);

	void setName(const std::string& theName) { name = theName; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getDefaultGood() const { return defaultGood; }
	[[nodiscard]] const auto& getGoodsFulfillment() const { return goodsFulfillment; }

  private:
	void registerKeys();
	parser goodsParser;
	GoodsFulfillment goodsFulfillmentBuilder;

	std::string name;
	std::string defaultGood;
	std::map<std::string, GoodsFulfillment> goodsFulfillment; // goods type to fulfillment level
};
} // namespace V3

#endif // V3_POP_NEED_H