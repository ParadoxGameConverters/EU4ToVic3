#ifndef NATION_MERGE_MAPPER
#define NATION_MERGE_MAPPER
#include "MergeBlock.h"
#include "Parser.h"

namespace mappers
{
class NationMergeMapper: commonItems::parser
{
  public:
	NationMergeMapper() = default;

	void loadNationMerge();
	void loadNationMerge(std::istream& theStream);

	[[nodiscard]] const auto& getMergeBlocks() const { return mergeBlocks; }
	[[nodiscard]] auto getMergeDaimyos() const { return mergeDaimyos; }

  private:
	void registerKeys();

	std::vector<MergeBlock> mergeBlocks;
	bool mergeDaimyos = false;
};
} // namespace mappers

#endif // NATION_MERGE_MAPPER
