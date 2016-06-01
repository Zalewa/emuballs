#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>

namespace Machine
{

class ArmMachine;

class IllegalOpcodeError : std::runtime_error
{
public:
	using runtime_error::runtime_error;
};

class ArmOpcode
{
public:
	ArmOpcode(uint32_t code);
	virtual ~ArmOpcode() = default;

	uint32_t code() const;
	void execute(ArmMachine &machine);

protected:
	virtual void run() = 0;

private:
	uint32_t m_code;
};

typedef std::unique_ptr<ArmOpcode> ArmOpcodePtr;

}
