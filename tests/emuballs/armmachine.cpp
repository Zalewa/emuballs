/*
 * Copyright 2017 Zalewa <zalewapl@gmail.com>.
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
#define BOOST_TEST_MODULE armmachine
#include <boost/test/unit_test.hpp>
#include "arm_program_fixture.hpp"

BOOST_AUTO_TEST_CASE(machine_copy_ctor)
{
	ArmProgramFixture machine1;
	machine1.load(std::begin(fibonacciCode), std::end(fibonacciCode));
	machine1.r(0, 18);

	// Run a while to modify the state.
	for (int i = 0; i < 30; ++i)
		machine1.machine.cycle();

	// Also screw around with memory.
	machine1.machine.memory().putWord(0x1234, 0xcafe);
	machine1.machine.memory().putWord(0xffff, 0xbeba);

	// Now copy and see if state is the same.
	Emuballs::Arm::Machine machine2 = machine1.machine;
	for (int i = 0; i <= 15; ++i)
		BOOST_CHECK_EQUAL(machine1.r(i), machine2.cpu().regs()[i]);
	BOOST_CHECK_EQUAL(machine2.memory().word(0x1234), 0xcafe);
	BOOST_CHECK_EQUAL(machine2.memory().word(0xffff), 0xbeba);

	// Now run the machine1 to the end and see if state is different.
	Emuballs::Arm::regval machine2r0 = machine2.cpu().regs()[0];
	machine1.runProgram();
	machine1.machine.memory().putWord(0x1234, 0xaaaa);
	BOOST_CHECK_NE(machine1.r(0), machine2.cpu().regs()[0]);
	BOOST_CHECK_EQUAL(machine2r0, machine2.cpu().regs()[0]);
	BOOST_CHECK_EQUAL(machine2.memory().word(0x1234), 0xcafe);
}
