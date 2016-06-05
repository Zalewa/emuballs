#pragma once

#include <bitset>
#include "dptr.hpp"

namespace Machine
{

namespace Arm
{

class ArmFlags
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

class ArmCpu
{
public:
	ArmFlags &flags();

private:
	DPtr<ArmCpu> d;
};

class ArmMachine
{
public:
	ArmCpu &cpu();

private:
	DPtr<ArmMachine> d;
};

} // namespace Arm

} // namespace Machine
