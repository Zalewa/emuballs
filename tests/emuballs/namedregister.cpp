/*
 * Copyright 2016 Zalewa <zalewapl@gmail.com>.
 *
 * This file is part of Emuballs.
 *
 * Emuballs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Emuballs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Emuballs.  If not, see <http://www.gnu.org/licenses/>.
 */
#define BOOST_TEST_MODULE namedregister
#include <boost/test/unit_test.hpp>
#include "src/emuballs/registerset.hpp"
#include "src/emuballs/regval.hpp"

using namespace Emuballs;

BOOST_AUTO_TEST_CASE(nameMatchSingle)
{
	NamedRegister reg("r0", 0);
	BOOST_CHECK(reg.isNameMatch("r0"));
	BOOST_CHECK(reg.isNameMatch("R0"));
	BOOST_CHECK(!reg.isNameMatch("r1"));
	BOOST_CHECK(!reg.isNameMatch("r0 "));
	BOOST_CHECK(!reg.isNameMatch(" R0"));
	BOOST_CHECK(!reg.isNameMatch(""));
	BOOST_CHECK(!reg.isNameMatch("\0"));
	BOOST_CHECK(!reg.isNameMatch("x0"));
	BOOST_CHECK(!reg.isNameMatch("R1"));
}

BOOST_AUTO_TEST_CASE(nameMatchAliases)
{
	NamedRegister reg(std::list<std::string>{"r15", "pc"}, 0);
	BOOST_CHECK(reg.isNameMatch("r15"));
	BOOST_CHECK(reg.isNameMatch("R15"));
	BOOST_CHECK(reg.isNameMatch("pc"));
	BOOST_CHECK(reg.isNameMatch("Pc"));
	BOOST_CHECK(reg.isNameMatch("pC"));
	BOOST_CHECK(reg.isNameMatch("PC"));
	BOOST_CHECK(!reg.isNameMatch("r1"));
	BOOST_CHECK(!reg.isNameMatch("r15 "));
	BOOST_CHECK(!reg.isNameMatch(" R15"));
	BOOST_CHECK(!reg.isNameMatch(""));
	BOOST_CHECK(!reg.isNameMatch("\0"));
	BOOST_CHECK(!reg.isNameMatch("x0"));
	BOOST_CHECK(!reg.isNameMatch("R1"));
}
