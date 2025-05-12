#ifndef FLAG_COLOR_LOADER_H
#define FLAG_COLOR_LOADER_H
#include "Color.h"
#include "Parser.h"

namespace V3
{
class FlagColorLoader: commonItems::parser
{
  public:
	FlagColorLoader() = default;
	void loadFlagColors(const std::string& filePath);

	[[nodiscard]] auto getNumFlagColors() const { return flagColorMapping.size(); }
	[[nodiscard]] std::optional<commonItems::Color> getFlagColorByIndex(int index) const;

  private:
	void registerKeys();

	std::vector<commonItems::Color> flagColorMapping;
};
}; // namespace V3

#endif // FLAG_COLOR_LOADER_H
