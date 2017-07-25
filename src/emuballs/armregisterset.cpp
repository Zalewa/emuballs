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
#include "armregisterset.hpp"

#include "emuballs/regval.hpp"

#include "armmachine.hpp"
#include "errors_private.hpp"
#include <string>

using namespace Emuballs;
using namespace Emuballs::Arm;

namespace Emuballs
{

DClass<Emuballs::Arm::NamedRegisterSet>
{
public:
	Machine *machine;
};

DPointeredNoCopy(Emuballs::Arm::NamedRegisterSet);

}

constexpr auto GENERAL_PURPOSE_REGISTERS = 10;

NamedRegisterSet::NamedRegisterSet(Machine &machine)
{
	d->machine = &machine;
}

std::list<NamedRegister> NamedRegisterSet::registers() const
{
	auto &regs = d->machine->cpu().regs();
	std::list<NamedRegister> mapping;
	for (int i = 0; i < GENERAL_PURPOSE_REGISTERS; ++i)
		mapping.push_back(NamedRegister("r" + std::to_string(i), regs[i]));
	mapping.push_back(NamedRegister(std::list<std::string> {"sl", "r10"}, regs[10]));
	mapping.push_back(NamedRegister(std::list<std::string> {"fp", "r11"}, regs[11]));
	mapping.push_back(NamedRegister(std::list<std::string> {"ip", "r12"}, regs[12]));
	mapping.push_back(NamedRegister(std::list<std::string> {"sp", "r13"}, regs[13]));
	mapping.push_back(NamedRegister(std::list<std::string> {"lr", "r14"}, regs[14]));
	mapping.push_back(NamedRegister(std::list<std::string> {"pc", "r15"}, regs[15]));
	return mapping;
}

void NamedRegisterSet::setReg(const std::string &name, const RegVal &value)
{
	auto namedRegs = registers();
	int index = 0;
	for (auto &namedReg : namedRegs)
	{
		if (namedReg.isNameMatch(name))
		{
			d->machine->cpu().regs()[index] = value;
			return;
		}
		++index;
	}
	throw UnknownRegisterError("unknown register " + name);
}
