#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>

namespace Machine
{

namespace Arm
{

class Machine;

class Opcode
{
public:
	Opcode(uint32_t code);
	virtual ~Opcode() = default;

	uint32_t code() const;
	void execute(Machine &machine);
	virtual void validate();

protected:
	virtual void run(Machine &machine) = 0;

private:
	uint32_t m_code;
};

typedef std::unique_ptr<Opcode> OpcodePtr;
typedef std::function<OpcodePtr(uint32_t)> OpFactory;

}

}
