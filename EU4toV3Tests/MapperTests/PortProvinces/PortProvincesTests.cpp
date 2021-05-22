#include "PortProvinces/PortProvinces.h"
#include "gtest/gtest.h"

TEST(Mappers_PortProvincesTests, allowedProvinceCanBePinged)
{
	std::stringstream input;
	input << "allowlist = { 1 2 3 }\n";
	const mappers::PortProvinces mapper(input);

	ASSERT_TRUE(mapper.isProvinceIDAllowed(1));
	ASSERT_TRUE(mapper.isProvinceIDAllowed(2));
	ASSERT_TRUE(mapper.isProvinceIDAllowed(3));
}

TEST(Mappers_PortProvincesTests, deniedProvinceCanBePinged)
{
	std::stringstream input;
	input << "denylist = { 1 2 3 }\n";
	const mappers::PortProvinces mapper(input);

	ASSERT_TRUE(mapper.isProvinceIDDenied(1));
	ASSERT_TRUE(mapper.isProvinceIDDenied(2));
	ASSERT_TRUE(mapper.isProvinceIDDenied(3));
}

TEST(Mappers_PortProvincesTests, nonListedProvinceReturnsFalse)
{
	std::stringstream input;
	input << "allowlist = { 1 2 3 }\n";
	input << "denylist = { 4 5 6 }\n";
	const mappers::PortProvinces mapper(input);

	ASSERT_FALSE(mapper.isProvinceIDDenied(1));
	ASSERT_FALSE(mapper.isProvinceIDDenied(2));
	ASSERT_FALSE(mapper.isProvinceIDDenied(3));
	ASSERT_FALSE(mapper.isProvinceIDAllowed(4));
	ASSERT_FALSE(mapper.isProvinceIDAllowed(5));
	ASSERT_FALSE(mapper.isProvinceIDAllowed(6));
	
	ASSERT_FALSE(mapper.isProvinceIDAllowed(7));
	ASSERT_FALSE(mapper.isProvinceIDDenied(7));
}
