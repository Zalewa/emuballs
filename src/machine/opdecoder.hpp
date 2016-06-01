#pragma once

#include <cstdint>
#include <functional>
#include <istream>
#include "armopcode.hpp"

namespace Machine
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
	typedef std::function<ArmOpcodePtr(uint32_t)> OpFactory;

	std::unique_ptr<ArmOpcode> next(std::istream &);
};

}
