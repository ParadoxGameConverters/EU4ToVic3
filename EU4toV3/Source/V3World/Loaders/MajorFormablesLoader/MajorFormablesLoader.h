#ifndef MAJOR_FORMABLES_LOADER_H
#define MAJOR_FORMABLES_LOADER_H
#include "MajorFormablesEntry.h"
#include "Parser.h"

namespace V3
{
class MajorFormablesLoader: commonItems::parser
{
  public:
	MajorFormablesLoader() = default;
	void loadMajorFormables(const std::string& filePath);

	[[nodiscard]] const auto& getMajorFormables() const { return majorFormables; }
	void setFormables(const std::map<std::string, MajorFormablesEntry>& theFormables) { majorFormables = theFormables; }

  private:
	void registerKeys();

	std::map<std::string, MajorFormablesEntry> majorFormables;
};
} // namespace V3
#endif // MAJOR_FORMABLES_LOADER_H