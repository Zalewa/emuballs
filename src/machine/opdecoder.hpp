#pragma once

#include <cstdint>
#include <istream>
#include "armopcode.hpp"

namespace Machine
{

namespace Arm
{

class ArmOpcode;


class OpDecodeError : public std::runtime_error
{
public:
	using runtime_error::runtime_error;
};


class ArmOpDecoder
{
public:
	ArmOpcodePtr next(std::istream &);
};

}

}
