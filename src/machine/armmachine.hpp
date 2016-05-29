#pragma once

#include <bitset>
#include "dptr.hpp"

namespace Machine
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

	uint32_t dump() const;
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

}
