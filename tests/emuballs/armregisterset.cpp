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
#define BOOST_TEST_MODULE armregisterset
#include <boost/test/unit_test.hpp>
#include "emuballs/regval.hpp"
#include "src/emuballs/armmachine.hpp"
#include "src/emuballs/armregisterset.hpp"
#include "src/emuballs/errors_private.hpp"

using namespace Emuballs::Arm;

struct Fixture
{
	Machine machine;
};

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(allNamedRegs)
{
	auto &regs = machine.cpu().regs();
	for (int i = 0; i < NUM_CPU_REGS; ++i)
		regs.set(i, (i + 1) * 4);
	NamedRegisterSet namedRegs(machine);
	int index = 0;
	for (auto &namedReg : namedRegs.registers())
	{
		BOOST_CHECK_EQUAL(regs[index], static_cast<regval>(namedReg.value()));
		++index;
	}
}

BOOST_AUTO_TEST_CASE(specificRegGet)
{
	auto &regs = machine.cpu().regs();
	regs.set(15, 0xcafebeef);
	regs.set(14, 1000);
	NamedRegisterSet namedRegs(machine);
	BOOST_CHECK_EQUAL(0xcafebeef, namedRegs.reg("r15"));
	BOOST_CHECK_EQUAL(0xcafebeef, namedRegs.reg("pc"));
	BOOST_CHECK_EQUAL(1000, namedRegs.reg("lr"));
}

BOOST_AUTO_TEST_CASE(specificRegSet)
{
	NamedRegisterSet namedRegs(machine);
	namedRegs.setReg("pc", 354);
	namedRegs.setReg("r0", 0xbeefcafe);
	auto &regs = machine.cpu().regs();
	BOOST_CHECK_EQUAL(354, regs[15]);
	BOOST_CHECK_EQUAL(0xbeefcafe, regs[0]);
}

BOOST_AUTO_TEST_CASE(unknownRegGet)
{
	NamedRegisterSet namedRegs(machine);
	BOOST_CHECK_THROW(namedRegs.reg("r200"), Emuballs::UnknownRegisterError);
	BOOST_CHECK_THROW(namedRegs.reg(""), Emuballs::UnknownRegisterError);
	BOOST_CHECK_THROW(namedRegs.reg("r0 "), Emuballs::UnknownRegisterError);
}

BOOST_AUTO_TEST_CASE(unknownRegSet)
{
	NamedRegisterSet namedRegs(machine);
	BOOST_CHECK_THROW(namedRegs.setReg("r200", 1), Emuballs::UnknownRegisterError);
	BOOST_CHECK_THROW(namedRegs.setReg("", -1), Emuballs::UnknownRegisterError);
	BOOST_CHECK_THROW(namedRegs.setReg("r0 ", 0), Emuballs::UnknownRegisterError);
}


BOOST_AUTO_TEST_SUITE_END()
