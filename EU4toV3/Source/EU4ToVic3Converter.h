#ifndef EU4TOVIC2_CONVERTER_H
#define EU4TOVIC2_CONVERTER_H

#include <string>

namespace mappers {
	class VersionParser;
}

void convertEU4ToVic2(const mappers::VersionParser& versionParser);
void deleteExistingOutputFolder();

#endif // EU4TOVIC2_CONVERTER_H
