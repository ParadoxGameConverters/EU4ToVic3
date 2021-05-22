#ifndef EU4_MERGE_BLOCK_H
#define EU4_MERGE_BLOCK_H
#include "ConvenientParser.h"
#include <set>

namespace EU4
{
class MergeBlock: commonItems::convenientParser
{
  public:
	explicit MergeBlock(std::istream& theStream);

	[[nodiscard]] const auto& getMaster() const { return master; }
	[[nodiscard]] const auto& getSlaves() const { return slaves; }
	[[nodiscard]] auto getMerge() const { return merge; }

  private:
	void registerKeys();

	std::string master;
	std::set<std::string> slaves;
	bool merge = false;
};
} // namespace EU4

#endif // EU4_MERGE_BLOCK_H
