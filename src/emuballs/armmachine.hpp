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
#pragma once

#include "armcpu.hpp"
#include "memory.hpp"
#include "dptr_impl.hpp"

namespace Emuballs
{

namespace Arm
{

class Machine
{
public:
	Machine();
	Machine(const Machine &other);
	Machine(Machine && other) noexcept;
	Machine &operator=(Machine other);
	friend void swap(Emuballs::Arm::Machine &a, Emuballs::Arm::Machine &b) noexcept
{
	using std::swap;

	swap(a.d, b.d);
	swap(a._cpu, b._cpu);
	swap(a._memory, b._memory);
	a.adjustPointers();
	b.adjustPointers();
}

	Cpu &cpu()
	{
		return _cpu;
	}

	const Cpu &cpu() const
	{
		return _cpu;
	}

	TrackedMemory memory()
	{
		return TrackedMemory(_memory);
	}

	Memory &untrackedMemory()
	{
		return _memory;
	}

	const Memory &untrackedMemory() const
	{
		return _memory;
	}

	void cycle();

private:
	Cpu _cpu;
	Memory _memory;
	DPtr<Machine> d;

	void adjustPointers() noexcept;
};

} // namespace Arm

} // namespace Emuballs
