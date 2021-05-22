#ifndef BUCKET_LIST_H
#define BUCKET_LIST_H

#include <random>
#include <vector>
#include "../../V3World/Province/Province.h"
#include "Bucket.h"
#include "Parser.h"

namespace mappers
{
	// Helper classes for RGO shuffle.
	class BucketList : commonItems::parser
	{
	public:
		BucketList();

		[[nodiscard]] auto empty() const { return buckets.empty(); }

		void putInBucket(std::shared_ptr<V2::Province> prov);
		void shuffle();

	private:
		std::vector<Bucket> buckets;
		std::default_random_engine shuffler;
	};
}

#endif // BUCKET_LIST_H
