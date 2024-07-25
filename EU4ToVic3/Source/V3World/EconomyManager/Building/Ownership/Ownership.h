#ifndef V3_OWNERSHIP_H
#define V3_OWNERSHIP_H
#include <string>

namespace V3
{
struct OwnershipData
{
	std::string type;
	double weight = 0.0;
	double colonialFrac = 0.0;
	double financialCenterFrac = 0.0;
	bool recognized = false;

	std::partial_ordering operator<=>(const OwnershipData&) const = default;
};
class Ownership: commonItems::parser
{
  public:
	Ownership(std::istream& theStream);

	[[nodiscard]] const auto& getOwnership() const { return ownership; }

  private:
	void registerKeys();

	OwnershipData ownership;
};
} // namespace V3

#endif // V3_OWNERSHIP_H
