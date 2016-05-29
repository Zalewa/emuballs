#pragma once

namespace Machine
{

class Machine;

class OpCode
{
public:
	virtual ~OpCode() = default;

	virtual void execute(Machine &machine) = 0;
};

}
