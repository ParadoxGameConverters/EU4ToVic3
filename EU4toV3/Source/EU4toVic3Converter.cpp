#include "EU4ToVic3Converter.h"
#include "Configuration.h"
#include "EU4World/World.h"
#include "Log.h"
#include "V3World/V3World.h"

void convertEU4ToVic3(const mappers::ConverterVersion& converterVersion)
{
	Log(LogLevel::Progress) << "0 %";
	const auto theConfiguration = std::make_shared<Configuration>();
	Log(LogLevel::Progress) << "4 %";

	Log(LogLevel::Progress) << "5 %";

	const EU4::World sourceWorld(theConfiguration);
	V2::World destWorld(theConfiguration, sourceWorld, converterVersion);

	Log(LogLevel::Info) << "* Conversion complete *";
	Log(LogLevel::Progress) << "100 %";
}
