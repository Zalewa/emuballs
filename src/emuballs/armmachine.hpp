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

#include <bitset>
#include <limits>
#include <vector>
#include "dptr_impl.hpp"

namespace Emuballs
{

class Memory;
class TrackedMemory;

namespace Arm
{

constexpr auto NUM_CPU_REGS = 16;

constexpr auto INSTRUCTION_SIZE = 4;
constexpr auto PREFETCH_SIZE = INSTRUCTION_SIZE * 2;

typedef uint32_t regval;
constexpr regval REGVAL_MAX = std::numeric_limits<regval>::max();
constexpr regval REGVAL_HIGHBIT = 0x80000000;

typedef uint32_t cpumode;

class RegisterSet
{
public:
	RegisterSet();

	void sp(const regval &value);
	const regval &sp() const;

	void lr(const regval &value);
	const regval &lr() const;

	void pc(const regval &value);
	const regval &pc() const;

	void set(int idx, const regval &value);
	const regval &operator[](int idx) const;

	void resetPcChanged();
	bool wasPcChanged() const;

private:
	std::vector<regval> regs;
	bool pcChanged;
};


class Flags
{
public:
	enum Bit
	{
		Overflow = 28, /// N
		Carry = 29, /// C
		Zero = 30, /// Z
		Negative = 31 /// N
	};

	constexpr static auto F_OVERFLOW = 1 << Overflow;
	constexpr static auto F_CARRY = 1 << Carry;
	constexpr static auto F_ZERO = 1 << Zero;
	constexpr static auto F_NEGATIVE = 1 << Negative;

	regval dump() const;
	void store(regval bitset);
	void set(Bit bit, bool state);
	bool test(Bit bit) const;

	bool carry() const;
	bool carry(bool);

	bool overflow() const;
	bool overflow(bool);

	bool zero() const;
	bool zero(bool);

	bool negative() const;
	bool negative(bool);

	cpumode cpuMode() const;

private:
	std::bitset<32> bits;
};

class Cpu
{
public:
	/**
	 * @brief CPSR flags.
	 */
	const Flags &flags() const;
	Flags &flags();

	/**
	 * @brief SPSR flags for current cpumode.
	 */
	const Flags &flagsSpsr() const;
	Flags &flagsSpsr();

	/**
	 * @brief SPSR flags for selected cpumode.
	 */
	const Flags &flagsSpsr(cpumode mode) const;
	Flags &flagsSpsr(cpumode mode);

	const RegisterSet &regs() const;
	RegisterSet &regs();

private:
	DPtr<Cpu> d;
};

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
