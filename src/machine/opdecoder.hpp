#pragma once

#include <cstdint>
#include <istream>
#include "armopcode.hpp"

namespace Machine
{

namespace Arm
{

class Opcode;


class OpDecodeError : public std::runtime_error
{
public:
	using runtime_error::runtime_error;
};


class OpDecoder
{
public:
	OpcodePtr next(std::istream &);
};

}

}
