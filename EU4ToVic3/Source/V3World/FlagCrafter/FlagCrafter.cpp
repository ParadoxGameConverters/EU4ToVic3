#include "FlagCrafter.h"
#include "CountryManager/EU4Country.h"
#include "CountryMapper/CountryMapper.h"
#include "FlagNameLoader/FlagNameLoader.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "PoliticalManager/Country/Country.h"
#include "targa.h"

void V3::FlagCrafter::loadAvailableFlags(const std::string& blankModPath, const std::string& vanillaPath)
{
	Log(LogLevel::Info) << "-> Loading Available Flags.";

	loadKnownFlags(blankModPath, vanillaPath);
	filterKnownFlags();

	Log(LogLevel::Info) << "<> Loaded " << knownFlags.size() << " available flags for " << availableFlags.size() << " entitites, and "
							  << knownVanillaFlags.size() << " vanilla flagsets.";
}

void V3::FlagCrafter::loadCustomColors(const std::string& filePath)
{
	flagColorLoader.loadFlagColors(filePath);
}

void V3::FlagCrafter::loadKnownFlags(const std::string& blankModPath, const std::string& vanillaPath)
{
	FlagNameLoader flagNameLoader;
	flagNameLoader.loadKnownFlags(blankModPath); // we're loading our COA DEFINITIONS, not flag definitions!
	knownFlags = flagNameLoader.getKnownFlags();
	FlagNameLoader vanillaFlagNameLoader;
	vanillaFlagNameLoader.loadKnownFlags(vanillaPath); // Now we're loading vanilla flag DEFINITIONS for tags, directly.
	knownVanillaFlags = vanillaFlagNameLoader.getKnownFlags();
}

void V3::FlagCrafter::filterKnownFlags()
{
	for (const auto& flag: knownFlags)
	{
		auto flagName = flag;
		FLAGTYPE flagType = Default;

		if (flagName.starts_with("legacy_") && flagName.size() > 7)
			flagName = flagName.substr(7, flagName.size());
		if (flag.ends_with("_republic") && flag.size() > 10)
		{
			flagName = flagName.substr(0, flagName.size() - 9);
			flagType = Republic;
		}
		else if (flag.ends_with("_monarchy") && flag.size() > 10)
		{
			flagName = flagName.substr(0, flagName.size() - 9);
			flagType = Monarchy;
		}
		else if (flag.ends_with("_communist") && flag.size() > 10)
		{
			flagName = flagName.substr(0, flagName.size() - 10);
			flagType = Communist;
		}
		else if (flag.ends_with("_fascist") && flag.size() > 10)
		{
			flagName = flagName.substr(0, flagName.size() - 8);
			flagType = Fascist;
		}

		if (!availableFlags.contains(flagName))
			availableFlags.emplace(flagName, std::map<FLAGTYPE, std::string>{});
		availableFlags.at(flagName).emplace(flagType, flag);
	}
}

std::optional<std::map<V3::FlagCrafter::FLAGTYPE, std::string>> V3::FlagCrafter::getFlagsForEntity(const std::string& name)
{
	if (const auto& itr = availableFlags.find(name); itr != availableFlags.end() && !spentFlags.contains(name))
	{
		spentFlags.emplace(name);
		return itr->second;
	}
	// Now get creative.
	if (const auto& itr = availableFlags.find("e_" + name); itr != availableFlags.end() && !spentFlags.contains("e_" + name))
	{
		spentFlags.emplace("e_" + name);
		return itr->second;
	}
	if (const auto& itr = availableFlags.find("k_" + name); itr != availableFlags.end() && !spentFlags.contains("k_" + name))
	{
		spentFlags.emplace("k_" + name);
		return itr->second;
	}
	if (const auto& itr = availableFlags.find("d_" + name); itr != availableFlags.end() && !spentFlags.contains("d_" + name))
	{
		spentFlags.emplace("d_" + name);
		return itr->second;
	}
	if (const auto& itr = availableFlags.find("c_" + name); itr != availableFlags.end() && !spentFlags.contains("c_" + name))
	{
		spentFlags.emplace("c_" + name);
		return itr->second;
	}

	return std::nullopt;
}

void V3::FlagCrafter::distributeAvailableFlags(const std::map<std::string, std::shared_ptr<Country>>& countries,
	 const mappers::CountryMapper& countryMapper,
	 const commonItems::ModFilesystem& eu4ModFS)
{
	Log(LogLevel::Info) << "-> Distributing Available Flags.";

	// prep the battleground.
	if (!commonItems::DeleteFolder("flags.tmp"))
		throw std::runtime_error("Could not delete flags.tmp folder!");
	if (!commonItems::TryCreateFolder("flags.tmp"))
		throw std::runtime_error("Could not create flags.tmp folder!");

	auto flagCodeCounter = 0;
	auto tagCounter = 0;
	auto nameCounter = 0;
	auto vanillaCounter = 0;
	auto eu4Counter = 0;

	for (const auto& [tag, country]: countries)
	{
		// are we already set?
		if (knownVanillaFlags.contains(tag))
		{
			++vanillaCounter;
			continue;
		}

		// do we have a flagcode? That'd be super helpful.
		if (const auto& flagCode = countryMapper.getFlagCode(tag); flagCode && tryAssigningFlagViaValue(country, *flagCode))
		{
			++flagCodeCounter;
			continue;
		}

		// try straight tag.
		if (tryAssigningFlagViaValue(country, tag))
		{
			++tagCounter;
			continue;
		}

		// umm. english name? yes?
		auto name = country->getName("english");
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (tryAssigningFlagViaValue(country, name))
		{
			++nameCounter;
			continue;
		}

		// do we need special attention?
		if (country->getProcessedData().customColors)
		{
			craftCustomFlag(country);
			continue;
		}

		// Can we use EU4 flag?
		if (tryAssigningEU4Flag(country, eu4ModFS))
		{
			++eu4Counter;
		}
	}

	Log(LogLevel::Info) << "<> Distributed flags for " << vanillaCounter + flagCodeCounter + tagCounter + nameCounter << " out of " << countries.size()
							  << " countries.";
	Log(LogLevel::Debug) << "Specifically, " << flagCodeCounter << " via flag codes, " << tagCounter << " via tag matches, " << nameCounter
								<< " via name matches, " << eu4Counter << " via EU4 flags, and " << vanillaCounter << " have vanilla flags.";
}

bool V3::FlagCrafter::tryAssigningEU4Flag(const std::shared_ptr<Country>& country, const commonItems::ModFilesystem& eu4ModFS)
{
	// Do we have EU4 nation and does it have a flag?
	if (!country->getSourceCountry())
		return false;

	const auto& eu4Tag = country->getSourceCountry()->getTag();

	// These would be full-path files. Let's trim and match.
	const auto& eu4Flags = eu4ModFS.GetAllFilesInFolder("gfx/flags/");

	std::string eu4FlagAbsolutePath;
	for (const auto& incomingEU4Flag: eu4Flags)
	{
		if (trimExtension(trimPath(incomingEU4Flag)) == eu4Tag)
		{
			eu4FlagAbsolutePath = incomingEU4Flag;
			break;
		}
	}

	if (eu4FlagAbsolutePath.empty())
		return false;

	// Try and copy EU4 flag over to temp folder, rename to V3 tag.
	if (!commonItems::TryCopyFile(eu4FlagAbsolutePath, "flags.tmp/" + country->getTag() + ".tga"))
		return false;

	// finally, add coa/flag record for the copied eu4 flag.
	country->addFlag(Default, country->getTag());
	country->addCustomFlag(Default, country->getTag());

	return true;
}

bool V3::FlagCrafter::tryAssigningFlagViaValue(const std::shared_ptr<Country>& country, const std::string& value)
{
	// strip legacy if we have one.
	auto actualValue = value;
	if (actualValue.starts_with("legacy_") && value.size() > 7)
		actualValue = value.substr(7, value.size());

	if (const auto& match = getFlagsForEntity(actualValue); match)
	{
		country->setFlags(*match);
		return true;
	}
	return false;
}

void V3::FlagCrafter::craftCustomFlag(const std::shared_ptr<Country>& country)
{
	std::string baseFlagFolder = "blankMod/output/gfx/coat_of_arms/textured_emblems/";

	const auto& v3Tag = country->getTag();
	const auto& customColors = *country->getProcessedData().customColors;

	auto baseFlagIndex = customColors.flagIndex;
	auto baseFlagStr = std::to_string(baseFlagIndex);
	auto emblem = customColors.symbolIndex;
	auto flagColor = customColors.flagColors;
	if (!flagColor)
	{
		Log(LogLevel::Error) << v3Tag << "'s flag colors are missing.";
		return;
	}
	auto [r, g, b] = flagColor->getRgbComponents();

	auto colourCount = static_cast<int>(flagColorLoader.getNumFlagColors());

	if (r > colourCount || g > colourCount || b > colourCount)
	{
		Log(LogLevel::Error) << v3Tag << "'s flag has some invalid colors.";
		return;
	}

	// We have 5 flags for every tag
	for (auto i = 0; i < 5; i++)
	{
		if (baseFlagIndex == 0)
			baseFlagStr = "tricolor";

		// For custom/revolutionary flags we only create the fascist, absolutist and communist
		if (baseFlagIndex == 0 && i != 0 && i != 4)
			continue;

		const auto& suffix = flagFileSuffixes[i];
		const auto suffixExtension = suffix + ".tga";
		auto sourceEmblemPath = baseFlagFolder + "/eu4_custom_emblems/" + std::to_string(emblem) + suffixExtension;
		auto sourceFlagPath = baseFlagFolder + "/eu4_custom_bases/" + baseFlagStr + ".tga";

		if (const auto flagFileFound = commonItems::DoesFileExist(sourceFlagPath) && commonItems::DoesFileExist(sourceEmblemPath); flagFileFound)
		{
			auto destFlagPath = "flags.tmp/" + v3Tag + suffixExtension;

			auto rColor = flagColorLoader.getFlagColorByIndex(r);
			auto gColor = flagColorLoader.getFlagColorByIndex(g);
			auto bColor = flagColorLoader.getFlagColorByIndex(b);
			if (!rColor)
				rColor = commonItems::Color();
			if (!gColor)
				gColor = commonItems::Color();
			if (!bColor)
				bColor = commonItems::Color();
			auto success = createCustomFlag(*rColor, *gColor, *bColor, sourceEmblemPath, sourceFlagPath, destFlagPath);
			if (!success)
				Log(LogLevel::Error) << "Flagcrafting of " << destFlagPath << " failed!";
			else
			{
				if (i == 0)
				{
					country->addFlag(Default, v3Tag + suffix);
					country->addCustomFlag(Default, v3Tag + suffix);
				}
				else if (i == 1)
				{
					country->addFlag(Communist, v3Tag + suffix);
					country->addCustomFlag(Communist, v3Tag + suffix);
				}
				else if (i == 2)
				{
					country->addFlag(Fascist, v3Tag + suffix);
					country->addCustomFlag(Fascist, v3Tag + suffix);
				}
				else if (i == 3)
				{
					country->addFlag(Monarchy, v3Tag + suffix);
					country->addCustomFlag(Monarchy, v3Tag + suffix);
				}
				else if (i == 4)
				{
					country->addFlag(Republic, v3Tag + suffix);
					country->addCustomFlag(Republic, v3Tag + suffix);
				}
			}
		}
		else
		{
			if (!commonItems::DoesFileExist(sourceFlagPath))
				throw std::runtime_error("Could not find " + sourceFlagPath);
			throw std::runtime_error("Could not find " + sourceEmblemPath);
		}
	}
}

bool V3::FlagCrafter::createCustomFlag(const commonItems::Color& c1,
	 const commonItems::Color& c2,
	 const commonItems::Color& c3,
	 const std::string& emblemPath,
	 const std::string& basePath,
	 const std::string& targetPath)
{
	tga_image base;
	tga_image emblem;

	auto res = tga_read(&base, basePath.c_str());
	if (res)
	{
		Log(LogLevel::Error) << "Failed to create custom flag: could not open " << basePath;
		Log(LogLevel::Error) << "Error message from targa: " << tga_error(res);
		return false;
	}

	res = tga_read(&emblem, emblemPath.c_str());
	if (res)
	{
		Log(LogLevel::Error) << "Failed to create custom flag: could not open " << emblemPath;
		Log(LogLevel::Error) << "Error message from targa: " << tga_error(res);
		return false;
	}

	for (auto y = 0; y < base.height; y++)
	{
		for (auto x = 0; x < base.width; x++)
		{
			auto* targetAddress = tga_find_pixel(&base, x, y);

			uint8_t r = 0, g = 0, b = 0;

			res = tga_unpack_pixel(targetAddress, base.pixel_depth, &b, &g, &r, nullptr);
			if (res)
			{
				Log(LogLevel::Error) << "Failed to create custom flag: could not read pixel data";
				Log(LogLevel::Error) << "Error message from targa: " << tga_error(res);
				return false;
			}

			const uint8_t c = ~r;
			const uint8_t m = ~g;
			const uint8_t z = ~b;

			auto tr = int(m * c1.r()) + int(c * c2.r()) + int(z * c3.r());
			auto tg = int(m * c1.g()) + int(c * c2.g()) + int(z * c3.g());
			auto tb = int(m * c1.b()) + int(c * c2.b()) + int(z * c3.b());

			tr /= 255;
			tg /= 255;
			tb /= 255;

			uint8_t oRed = 0, oGreen = 0, oBlue = 0, oAlpha = 0;

			auto* targetOverlayAddress = tga_find_pixel(&emblem, x, y);
			if (targetOverlayAddress)
			{
				res = tga_unpack_pixel(targetOverlayAddress, emblem.pixel_depth, &oBlue, &oGreen, &oRed, &oAlpha);
				if (res)
				{
					Log(LogLevel::Error) << "Failed to create custom flag: could not read pixel data";
					Log(LogLevel::Error) << "Error message from targa: " << tga_error(res);
					return false;
				}

				tr = oRed * oAlpha / 255 + tr * (255 - oAlpha) / 255;
				tg = oGreen * oAlpha / 255 + tg * (255 - oAlpha) / 255;
				tb = oBlue * oAlpha / 255 + tb * (255 - oAlpha) / 255;
			}
			else
			{
				Log(LogLevel::Warning) << "No targetOverlayAddress, error with targa handling.";
			}

			res = tga_pack_pixel(targetAddress, base.pixel_depth, tb, tg, tr, 255);
			if (res)
			{
				Log(LogLevel::Error) << "Failed to create custom flag: could not write pixel data";
				Log(LogLevel::Error) << "Error message from targa: " << tga_error(res);
				return false;
			}
		}
	}

	res = tga_write(targetPath.c_str(), &base);
	if (res)
	{
		Log(LogLevel::Error) << "Failed to create custom flag: could not write to " << targetPath;
		Log(LogLevel::Error) << "Error message from targa: " << tga_error(res);
		return false;
	}

	return true;
}
