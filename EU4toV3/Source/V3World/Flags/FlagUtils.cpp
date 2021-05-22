#include "FlagUtils.h"
#include "Log.h"
#include "targa.h"

bool V2::createColonialFlag(const std::string& colonialOverlordPath, const std::string& colonialBasePath, const std::string& targetPath)
{
	tga_image ColonialBase;
	tga_image Corner;

	tga_result res = tga_read(&ColonialBase, colonialBasePath.c_str());
	if (0 != res)
	{
		LOG(LogLevel::Error) << "Failed to create colonial flag: could not open " << colonialBasePath;
		LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
		return false;
	}

	res = tga_read(&Corner, colonialOverlordPath.c_str());
	if (0 != res)
	{
		LOG(LogLevel::Error) << "Failed to create colonial flag: could not open " << colonialOverlordPath;
		LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
		return false;
	}

	for (auto y = 0; y < 31; y++)
	{
		for (auto x = 0; x < 45; x++)
		{
			auto* targetAddress = tga_find_pixel(&ColonialBase, x, y);

			uint8_t* sample[4];
			sample[0] = tga_find_pixel(&Corner, 2 * x, 2 * y);
			sample[1] = tga_find_pixel(&Corner, 2 * x + 1, 2 * y);
			sample[2] = tga_find_pixel(&Corner, 2 * x, 2 * y + 1);
			sample[3] = tga_find_pixel(&Corner, 2 * x + 1, 2 * y + 1);

			uint8_t b = 0, g = 0, r = 0;
			auto tb = 0, tg = 0, tr = 0;

			for (auto px = 0; px < 4; px++)
			{
				res = tga_unpack_pixel(sample[px], Corner.pixel_depth, &b, &g, &r, nullptr);
				if (res)
				{
					LOG(LogLevel::Error) << "Failed to create colonial flag: could not read pixel data";
					LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
					return false;
				}
				tb += b / 4;
				tg += g / 4;
				tr += r / 4;
			}

			res = tga_pack_pixel(targetAddress, ColonialBase.pixel_depth, tb, tg, tr, 255);
			if (res)
			{
				LOG(LogLevel::Error) << "Failed to create colonial flag: could not write pixel data";
				LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
				return false;
			}
		}
	}
	res = tga_write(targetPath.c_str(), &ColonialBase);
	if (0 != res)
	{
		LOG(LogLevel::Error) << "Failed to create colonial flag: could not write to " << targetPath;
		LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
		return false;
	}

	return true;
}

bool V2::createCustomFlag(const commonItems::Color& c1,
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
		LOG(LogLevel::Error) << "Failed to create custom flag: could not open " << basePath;
		LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
		return false;
	}

	res = tga_read(&emblem, emblemPath.c_str());
	if (res)
	{
		LOG(LogLevel::Error) << "Failed to create custom flag: could not open " << emblemPath;
		LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
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
				LOG(LogLevel::Error) << "Failed to create custom flag: could not read pixel data";
				LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
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
					LOG(LogLevel::Error) << "Failed to create custom flag: could not read pixel data";
					LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
					return false;
				}

				tr = oRed * oAlpha / 255 + tr * (255 - oAlpha) / 255;
				tg = oGreen * oAlpha / 255 + tg * (255 - oAlpha) / 255;
				tb = oBlue * oAlpha / 255 + tb * (255 - oAlpha) / 255;
			}
			else
			{
				LOG(LogLevel::Warning) << "No targetOverlayAddress, error with targa handling.";
			}

			res = tga_pack_pixel(targetAddress, base.pixel_depth, tb, tg, tr, 255);
			if (res)
			{
				LOG(LogLevel::Error) << "Failed to create custom flag: could not write pixel data";
				LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
				return false;
			}
		}
	}

	res = tga_write(targetPath.c_str(), &base);
	if (res)
	{
		LOG(LogLevel::Error) << "Failed to create custom flag: could not write to " << targetPath;
		LOG(LogLevel::Error) << "Error message from targa: " << tga_error(res);
		return false;
	}

	return true;
}
