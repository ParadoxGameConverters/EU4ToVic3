#ifndef CONVERTER_VERSION_H
#define CONVERTER_VERSION_H
#include "ConvenientParser.h"
#include "GameVersion.h"

namespace mappers
{
class ConverterVersion: commonItems::convenientParser
{
  public:
	ConverterVersion();
	explicit ConverterVersion(std::istream& theStream);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getVersion() const { return version; }
	[[nodiscard]] const auto& getDescription() const { return descriptionLine; }
	[[nodiscard]] const auto& getMinimalVersion() const { return minimalIncomingVersion; }

	friend std::ostream& operator<<(std::ostream& output, const ConverterVersion& versionParser);

  private:
	void registerKeys();

	std::string name;
	std::string version;
	std::string descriptionLine;
	GameVersion minimalIncomingVersion;
};
} // namespace mappers

#endif // CONVERTER_VERSION_H