// Hashing Tests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// #define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>
#include "stdafx.h"
#include "Keccak.h" // project being tested
#include "Hex.h"
#include <string>
#include <vector>
#include <sstream>

std::string hex_encode(std::string inpt, KeccakBase &k) {
	uint8_t* byte_array = reinterpret_cast<uint8_t*>(&inpt[0]);
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
	std::string raw_data = "A";
	std::string expected_value = "1c9ebd6caf02840a5b2b7f0fc870ec1db154886ae9fe621b822b14fd0bf513d6";
	Sha3 h(expected_value.length());
	BOOST_CHECK_EQUAL(hex_encode(raw_data, h), expected_value);

	h.reset();
	raw_data = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
	expected_value = "bde3f269175e1dcda13848278aa6046bd643cea85b84c8b8bb80952e70b6eae0";
	BOOST_CHECK_EQUAL(hex_encode(raw_data, h), expected_value);
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
