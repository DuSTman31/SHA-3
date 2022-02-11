// KeccakTests.cpp
//
#define BOOST_TEST_MODULE SHA3Tests
#include <boost/test/included/unit_test.hpp>
#include "../SHA-3/Keccak.h"
#include "../SHA-3/Keccak.cpp"
#include "../SHA-3/Hex.h"
#include "../SHA-3/HashFunction.h"
#include "../SHA-3/HashFunction.cpp"
#include <string>
#include <vector>
#include <sstream>

std::string hex_encode(std::string inpt, KeccakBase& k) {
	const uint8_t* byte_array = reinterpret_cast<const uint8_t*>(inpt.data());
	k.addData(byte_array, 0, inpt.length());
	std::vector<unsigned char> op = k.digest();
	std::ostringstream b;
	for (auto& oi : op)
	{
		Hex(oi, [&b](unsigned char a) { b << a; });
	}
	return b.str();
}

BOOST_AUTO_TEST_CASE(my_boost_test)
{
	// test cases from https://emn178.github.io/online-tools/sha3_256.html
	std::string raw_data("A");
	std::string expected_value("1c9ebd6caf02840a5b2b7f0fc870ec1db154886ae9fe621b822b14fd0bf513d6");
	// unsigned int hashWidth = expected_value.length() * 4;
	Sha3 h256(256);
	BOOST_CHECK_EQUAL(hex_encode(raw_data, h256), expected_value);

	h256.reset();
	raw_data = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
	expected_value = "9ce91e9413e1f14bb8ac791b08a5794b81e7133a47deebbc877f3db9e91d0de0";
	BOOST_CHECK_EQUAL(hex_encode(raw_data, h256), expected_value);

	expected_value = "755c33e0b5aa11f8cf6cc91eaaa2b9cd57e5029b4ed18976d7f619ff9898bd06c5483c82a349ec0cd28793968cb4312c3f240fdcffc49afe06835dc285d2080a";
	Sha3 h512(512);
	BOOST_CHECK_EQUAL(hex_encode(raw_data, h512), expected_value);
}
