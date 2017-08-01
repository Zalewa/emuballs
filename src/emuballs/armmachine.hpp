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
constexpr auto PREFETCH_INSTRUCTIONS = 2;
constexpr auto PREFETCH_SIZE = INSTRUCTION_SIZE * PREFETCH_INSTRUCTIONS;

typedef uint32_t regval;
constexpr regval REGVAL_MAX = std::numeric_limits<regval>::max();
constexpr regval REGVAL_HIGHBIT = 0x80000000;

typedef uint32_t cpumode;

class RegisterSet
{
public:
	RegisterSet();

	// Aliases to set() and operator[] should all be inline.

	inline void sp(const regval &value)
	{
		set(13, value);
	}
	inline const regval &sp() const
	{
		return (*this)[13];
	}

	inline void lr(const regval &value)
	{
		set(14, value);
	}
	inline const regval &lr() const
	{
		return (*this)[14];
	}

	inline void pc(const regval &value)
	{
		set(15, value);
	}
	inline const regval &pc() const
	{
		return (*this)[15];
	}

	void set(int idx, const regval &value)
	{
		if (idx == 15)
			pcChanged = true;
		regs[idx] = value;
	}

	const regval &operator[](int idx) const
	{
		return regs[idx];
	}

	inline void resetPcChanged()
	{
		pcChanged = false;
	}

	inline bool wasPcChanged() const
	{
		return pcChanged;
	}

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

	regval dump() const
	{
		return bits.to_ulong();
	}

	void store(regval bitset)
	{
		bits = std::bitset<32>(bitset);
	}

	void set(Bit bit, bool state)
	{
		bits.set(bit, state);
	}

	bool test(Bit bit) const
	{
		return bits.test(bit);
	}

	bool carry() const
	{
		return test(Carry);
	}

	bool carry(bool flag)
	{
		set(Carry, flag);
		return flag;
	}

	bool overflow() const
	{
		return test(Overflow);
	}

	bool overflow(bool flag)
	{
		set(Overflow, flag);
		return flag;
	}

	bool zero() const
	{
		return test(Zero);
	}

	bool zero(bool flag)
	{
		set(Zero, flag);
		return flag;
	}

	bool negative() const
	{
		return test(Negative);
	}

	bool negative(bool flag)
	{
		set(Negative, flag);
		return flag;
	}

	cpumode cpuMode() const
	{
		return dump() & 0x1f;
	}

private:
	std::bitset<32> bits;
};

class Cpu
{
public:
	/**
	 * @brief CPSR flags.
	 */
	const Flags &flags() const
	{
		return _flags;
	}

	Flags &flags()
	{
		return _flags;
	}

	/**
	 * @brief SPSR flags for current cpumode.
	 */
	const Flags &flagsSpsr() const
	{
		return flagsSpsr(_flags.cpuMode());
	}

	Flags &flagsSpsr()
	{
		return flagsSpsr(_flags.cpuMode());
	}

	/**
	 * @brief SPSR flags for selected cpumode.
	 */
	const Flags &flagsSpsr(cpumode mode) const
	{
		if (mode >= 32)
			throw std::out_of_range("cpumode must be <= 31, was: " + std::to_string(mode));
		return _storedFlags[mode];
	}

	Flags &flagsSpsr(cpumode mode)
	{
		return const_cast<Flags&>( static_cast<const Cpu&>(*this).flagsSpsr(mode) );
	}

	const RegisterSet &regs() const
	{
		return _regs;
	}

	RegisterSet &regs()
	{
		return _regs;
	}

private:
	Flags _flags;
	Flags _storedFlags[32];
	RegisterSet _regs;
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
