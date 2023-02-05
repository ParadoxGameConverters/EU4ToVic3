#ifndef LAW_LOADER_H
#define LAW_LOADER_H
#include "Law.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class LawLoader: commonItems::parser
{
  public:
	LawLoader() = default;
	void loadLaws(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] const auto& getLaws() const { return laws; }

  private:
	void registerKeys();

	std::map<std::string, Law> laws;
};
} // namespace V3
#endif // LAW_LOADER_H