#pragma once

#include <bitset>
#include <limits>
#include <vector>
#include "dptr.hpp"

namespace Machine
{

class Memory;

namespace Arm
{

constexpr auto PREFETCH_SIZE = 8;
constexpr auto INSTRUCTION_SIZE = 4;

typedef uint32_t regval;
constexpr regval REGVAL_MAX = std::numeric_limits<regval>::max();
constexpr regval REGVAL_HIGHBIT = 0x80000000;

typedef uint32_t cpumode;

class RegisterSet
{
public:
	RegisterSet();

	regval &sp();
	const regval &sp() const;

	regval &lr();
	const regval &lr() const;

	regval &pc();
	const regval &pc() const;

	regval &operator[](int idx);
	const regval &operator[](int idx) const;

private:
	std::vector<regval> regs;
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

	Memory &memory();
	const Memory &memory() const;

	void cycle();

private:
	DPtr<Machine> d;
};

} // namespace Arm

} // namespace Machine
