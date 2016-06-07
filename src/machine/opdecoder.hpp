#pragma once

#include <cstdint>
#include <istream>
#include "armopcode.hpp"

namespace Machine
{

namespace Arm
{

class Opcode;


class OpDecoder
{
public:
	OpcodePtr next(std::istream &);
	OpcodePtr decode(uint32_t instruction);
};

}

}
