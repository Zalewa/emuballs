#pragma once

#include "opcode.hpp"
#include <cstdint>

namespace Machine
{

class ArmOpCode : public OpCode
{
public:
	ArmOpCode(uint32_t code);
	virtual ~ArmOpCode() = default;

	void execute(Machine &machine);

protected:
	virtual void run() = 0;

private:
	uint32_t code;
};

}
