#ifndef EU4TOVIC3_CONVERTER_H
#define EU4TOVIC3_CONVERTER_H

namespace mappers {
	class VersionParser;
}

void convertEU4ToVic3(const mappers::VersionParser& versionParser);
void deleteExistingOutputFolder();

#endif // EU4TOVIC3_CONVERTER_H
