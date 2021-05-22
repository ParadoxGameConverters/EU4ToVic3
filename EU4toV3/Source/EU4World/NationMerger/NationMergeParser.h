#ifndef EU4_NATION_MERGE_PARSER_H
#define EU4_NATION_MERGE_PARSER_H
#include "MergeBlock.h"
#include "Parser.h"

namespace EU4
{
class NationMergeParser: commonItems::parser
{
  public:
	NationMergeParser();
	explicit NationMergeParser(std::istream& theStream);

	[[nodiscard]] const auto& getMergeBlocks() const { return mergeBlocks; }
	[[nodiscard]] auto getMergeDaimyos() const { return mergeDaimyos; }

  private:
	void registerKeys();

	std::vector<MergeBlock> mergeBlocks;
	bool mergeDaimyos = false;
};
} // namespace EU4

#endif // EU4_NATION_MERGE_PARSER_H
