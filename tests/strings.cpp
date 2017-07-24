/*
 * Copyright 2016 Zalewa <zalewapl@gmail.com>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#define BOOST_TEST_MODULE strings
#include <boost/test/unit_test.hpp>
#include "src/common/strings.hpp"


BOOST_AUTO_TEST_CASE(_compareCaseInsensitive)
{
	using namespace Strings;
	BOOST_CHECK_EQUAL(0, compareCaseInsensitive("a", "a"));
	BOOST_CHECK_EQUAL(0, compareCaseInsensitive("abc", "abc"));
	BOOST_CHECK_EQUAL(0, compareCaseInsensitive("AbC", "aBc"));
	BOOST_CHECK_EQUAL(0, compareCaseInsensitive("", ""));
	BOOST_CHECK_EQUAL(0, compareCaseInsensitive("jane has a cat", "jane has a cat"));
	BOOST_CHECK_EQUAL(0, compareCaseInsensitive("Jane Has a Cat", "jane has a cat"));
	BOOST_CHECK_EQUAL(0, compareCaseInsensitive("JANE HAS A CAT", "JANE HAS A CAT"));
	BOOST_CHECK_EQUAL(0, compareCaseInsensitive("jane has a cat", "JANE HAS A CAT"));
	BOOST_CHECK_EQUAL(0, compareCaseInsensitive(" ", " "));
	BOOST_CHECK_EQUAL(0, compareCaseInsensitive(
			"-=\\|+_1234567890!@#$%^&*()[]{};:\"',.<>/?",
			"-=\\|+_1234567890!@#$%^&*()[]{};:\"',.<>/?"));

	BOOST_CHECK_GT(compareCaseInsensitive("b", "a"), 0);
	BOOST_CHECK_GT(compareCaseInsensitive("abcdef", "abc"), 0);
	BOOST_CHECK_GT(compareCaseInsensitive("bbC", "ABc"), 0);
	BOOST_CHECK_GT(compareCaseInsensitive("x", ""), 0);
	BOOST_CHECK_GT(compareCaseInsensitive("jane has a dog", "jane has a cat"), 0);
	BOOST_CHECK_GT(compareCaseInsensitive("Jane Has a Dog", "jane has a cat"), 0);
	BOOST_CHECK_GT(compareCaseInsensitive("JANE HAS A DOG", "JANE HAS A CAT"), 0);
	BOOST_CHECK_GT(compareCaseInsensitive("jane has a dog", "JANE HAS A CAT"), 0);
	BOOST_CHECK_GT(compareCaseInsensitive("  ", " "), 0);
	BOOST_CHECK_GT(compareCaseInsensitive(
			"-=\\|+_1234567899!@#$%^&*()[]{};:\"',.<>/?",
			"-=\\|+_1234567890!@#$%^&*()[]{};:\"',.<>/?"), 0);

	BOOST_CHECK_LT(compareCaseInsensitive("a", "b"), 0);
	BOOST_CHECK_LT(compareCaseInsensitive("abc", "abcdef"), 0);
	BOOST_CHECK_LT(compareCaseInsensitive("ABc", "bbC"), 0);
	BOOST_CHECK_LT(compareCaseInsensitive("", "x"), 0);
	BOOST_CHECK_LT(compareCaseInsensitive("jane has a cat", "jane has a dog"), 0);
	BOOST_CHECK_LT(compareCaseInsensitive("Jane Has a Cat", "jane has a dog"), 0);
	BOOST_CHECK_LT(compareCaseInsensitive("JANE HAS A CAT", "JANE HAS A Dog"), 0);
	BOOST_CHECK_LT(compareCaseInsensitive("jane has a cat", "JANE HAS A DOG"), 0);
	BOOST_CHECK_LT(compareCaseInsensitive(" ", "  "), 0);
	BOOST_CHECK_LT(compareCaseInsensitive(
			"-=\\|+_1234567890!@#$%^&*()[]{};:\"',.<>/?",
			"-=\\|+_1234567899!@#$%^&*()[]{};:\"',.<>/?"), 0);
}

BOOST_AUTO_TEST_CASE(concat_string)
{
	{
		std::list<std::string> strList = { "Jane", "has", "a", "cat"};
		BOOST_CHECK_EQUAL("Jane has a cat", Strings::concat(strList, " "));
	}
	{
		std::list<std::string> strList;
		BOOST_CHECK_EQUAL("", Strings::concat(strList, " "));
	}
	{
		std::list<std::string> strList = { " Four words, one string " };
		BOOST_CHECK_EQUAL(" Four words, one string ", Strings::concat(strList, " "));
	}
	{
		std::list<std::string> strList = { "Jane", "", "Cat" };
		BOOST_CHECK_EQUAL("Jane  Cat", Strings::concat(strList, " "));
	}
	{
		std::list<std::string> strList = { "Jane", " ", "Cat" };
		BOOST_CHECK_EQUAL("Jane   Cat", Strings::concat(strList, " "));
	}
	{
		std::list<std::string> strList = { "Jane", "has", "a", "cat"};
		BOOST_CHECK_EQUAL("Jane---has---a---cat", Strings::concat(strList, "---"));
	}
	{
		std::vector<std::string> strList = { "Jane", "has", "a", "cat" };
		BOOST_CHECK_EQUAL("Jane has a cat", Strings::concat(strList, " "));
	}
	{
		std::vector<std::string> strList = { "Jane", "has", "a", "cat" };
		BOOST_CHECK_EQUAL("Janehasacat", Strings::concat(strList, ""));
	}
	{
		std::string strList[] = { "Jane", "has", "a", "cat" };
		BOOST_CHECK_EQUAL("Jane has a cat", Strings::concat(strList, " "));
	}
}

// I can't get the Boost Unit Testing Framework to compile with wchar_t*
// comparison functions.
#ifndef _WIN32
BOOST_AUTO_TEST_CASE(concat_wstring)
{
	{
		std::list<std::wstring> strList = { L"Jane", L"has", L"a", L"cat"};
		BOOST_CHECK_EQUAL(L"Jane has a cat", Strings::concat(strList, L" ").c_str());
	}
	{
		std::list<std::wstring> strList;
		BOOST_CHECK_EQUAL(L"", Strings::concat(strList, L" ").c_str());
	}
	{
		std::list<std::wstring> strList = { L" Four words, one string " };
		BOOST_CHECK_EQUAL(L" Four words, one string ", Strings::concat(strList, L" ").c_str());
	}
	{
		std::list<std::wstring> strList = { L"Jane", L"", L"Cat" };
		BOOST_CHECK_EQUAL(L"Jane  Cat", Strings::concat(strList, L" ").c_str());
	}
	{
		std::list<std::wstring> strList = { L"Jane", L" ", L"Cat" };
		BOOST_CHECK_EQUAL(L"Jane   Cat", Strings::concat(strList, L" ").c_str());
	}
	{
		std::list<std::wstring> strList = { L"Jane", L"has", L"a", L"cat"};
		BOOST_CHECK_EQUAL(L"Jane---has---a---cat", Strings::concat(strList, L"---").c_str());
	}
	{
		std::vector<std::wstring> strList = { L"Jane", L"has", L"a", L"cat" };
		BOOST_CHECK_EQUAL(L"Jane has a cat", Strings::concat(strList, L" ").c_str());
	}
	{
		std::vector<std::wstring> strList = { L"Jane", L"has", L"a", L"cat" };
		BOOST_CHECK_EQUAL(L"Janehasacat", Strings::concat(strList, L"").c_str());
	}
	{
		std::wstring strList[] = { L"Jane", L"has", L"a", L"cat" };
		BOOST_CHECK_EQUAL(L"Jane has a cat", Strings::concat(strList, L" ").c_str());
	}
}
#endif
