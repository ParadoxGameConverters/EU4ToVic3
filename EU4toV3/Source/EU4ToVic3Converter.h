#ifndef EU4TOVIC3_CONVERTER_H
#define EU4TOVIC3_CONVERTER_H
#include <memory>

namespace mappers {
	class ConverterVersion;
}

void convertEU4ToVic3(const std::shared_ptr<mappers::ConverterVersion>& converterVersion);

#endif // EU4TOVIC3_CONVERTER_H
