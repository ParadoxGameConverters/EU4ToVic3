#ifndef EU4_WORLD_H
#define EU4_WORLD_H
#include "Configuration.h"
#include "GameVersion.h"
#include "Parser.h"
#include <memory>

namespace EU4
{
class World: commonItems::parser
{
  public:
	World(const std::shared_ptr<Configuration>& theConfiguration);

  private:
	void registerKeys(const std::shared_ptr<Configuration>& theConfiguration);

	void verifySave(const std::string& saveGamePath);
	void verifySaveContents();
	bool uncompressSave(const std::string& saveGamePath);

	struct saveData
	{
		bool compressed = false;
		std::string metadata;
		std::string gamestate;
	};
	saveData saveGame;

	std::unique_ptr<GameVersion> version;

};
} // namespace EU4

#endif // EU4_WORLD_H
