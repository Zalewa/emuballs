#pragma once

#include <cstdint>
#include <stdexcept>

namespace Machine
{

class ArmMachine;

class IllegalOpCodeError : std::runtime_error
{
public:
	using runtime_error::runtime_error;
};

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
