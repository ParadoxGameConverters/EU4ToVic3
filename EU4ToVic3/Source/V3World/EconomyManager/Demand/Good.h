#ifndef V3_GOODS_H
#define V3_GOODS_H
#include "string"

namespace V3
{
class Good: commonItems::parser
{
  public:
	Good() = default;
	explicit Good(std::istream& theStream);

	void setName(const std::string& theName) { good = theName; }

	[[nodiscard]] const auto& getName() const { return good; }
	[[nodiscard]] const auto& getBasePrice() const { return cost; }
	[[nodiscard]] const auto& isLocal() const { return local; }

  private:
	void registerKeys();

	std::string good;
	int cost = 0;
	bool local = false;
};
} // namespace V3

#endif // V3_GOODS_H