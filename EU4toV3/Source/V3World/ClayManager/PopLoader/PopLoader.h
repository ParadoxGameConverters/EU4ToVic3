#ifndef POP_LOADER_H
#define POP_LOADER_H
#include "Parser.h"
#include "Pop.h"

namespace V3
{
class PopLoader: commonItems::parser
{
  public:
	PopLoader() = default;

	void loadPops(const std::string& v3Path);
	[[nodiscard]] const auto& getStatePops() const { return statePops; }

  private:
	void registerKeys();

	std::map<std::string, StatePops> statePops;
};
} // namespace V3
#endif // POP_LOADER_H