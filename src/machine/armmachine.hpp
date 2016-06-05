#pragma once

#include <bitset>
#include "dptr.hpp"

namespace Machine
{

namespace Arm
{

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

	uint32_t dump() const;
	void store(uint32_t bitset);
	void set(Bit bit, bool state);
	bool test(Bit bit) const;

private:
	std::bitset<32> bits;
};

class Cpu
{
public:
	Flags &flags();

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
