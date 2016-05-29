#pragma once

#include <cstdint>

namespace Machine
{

class ArmMachine;

class ArmOpCode
{
public:
	ArmOpCode(uint32_t code);
	virtual ~ArmOpCode() = default;

	void execute(ArmMachine &machine);

protected:
	virtual void run() = 0;

private:
	uint32_t code;
};

}
