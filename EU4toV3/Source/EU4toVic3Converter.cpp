#include "EU4ToVic3Converter.h"
#include "Configuration.h"
#include "EU4World/World.h"
#include "Log.h"
#include "V3World/V3World.h"

void convertEU4ToVic3(commonItems::ConverterVersion&& converterVersion)
{
	Log(LogLevel::Progress) << "0 %";
	const auto configuration = Configuration(converterVersion);
	Log(LogLevel::Info) << "<> Configuration imported.";
	Log(LogLevel::Progress) << "4 %";

	const EU4::World sourceWorld(configuration, converterVersion);
	V3::World destWorld(configuration, sourceWorld, std::move(converterVersion));

	Log(LogLevel::Notice) << "* Conversion complete *";
	Log(LogLevel::Progress) << "100 %";
}
