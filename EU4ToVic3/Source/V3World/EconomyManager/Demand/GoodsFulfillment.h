#ifndef V3_GOODS_FULFILLMENT_H
#define V3_GOODS_FULFILLMENT_H
#include "string"

namespace V3
{
struct GoodsFulfillment
{
	std::string good;
	double weight = 1.0;
	double maxShare = 1.0;
	double minShare = 0.0;

	std::partial_ordering operator<=>(const GoodsFulfillment&) const = default;
};
} // namespace V3

#endif // V3_GOODS_FULFILLMENT_H