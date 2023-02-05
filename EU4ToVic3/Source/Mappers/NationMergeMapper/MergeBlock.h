#ifndef MERGE_BLOCK
#define MERGE_BLOCK
#include "ConvenientParser.h"
#include <set>

namespace mappers
{
class MergeBlock: commonItems::convenientParser
{
  public:
	explicit MergeBlock(std::istream& theStream);

	[[nodiscard]] const auto& getMaster() const { return master; }
	[[nodiscard]] const auto& getSlaves() const { return slaves; }
	[[nodiscard]] auto shouldMerge() const { return merge; }

  private:
	void registerKeys();

	std::string master;
	std::set<std::string> slaves;
	bool merge = false;
};
} // namespace mappers

#endif // MERGE_BLOCK
