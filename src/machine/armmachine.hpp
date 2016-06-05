#pragma once

#include <bitset>
#include <vector>
#include "dptr.hpp"

namespace Machine
{

namespace Arm
{

constexpr auto PREFETCH_SIZE = 8;
constexpr auto INSTRUCTION_SIZE = 4;

typedef uint32_t regval;

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

private:
	std::bitset<32> bits;
};

class Cpu
{
public:
	const Flags &flags() const;
	Flags &flags();

	const RegisterSet &regs() const;
	RegisterSet &regs();

private:
	DPtr<Cpu> d;
};

class Machine
{
public:
	Cpu &cpu();

private:
	DPtr<Machine> d;
};

} // namespace Arm

} // namespace Machine
