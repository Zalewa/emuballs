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
#include "dptr_impl.hpp"

namespace Emuballs
{

class Memory;
class TrackedMemory;

namespace Arm
{

class Machine
{
public:
	Cpu &cpu();
	const Cpu &cpu() const;

	TrackedMemory memory();
	const TrackedMemory memory() const;

	Memory &untrackedMemory();
	const Memory &untrackedMemory() const;

	void cycle();

private:
	DPtr<Machine> d;
};

} // namespace Arm

} // namespace Emuballs
