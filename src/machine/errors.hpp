#pragma once

#include <stdexcept>

namespace Machine
{

class IllegalOpcodeError : std::runtime_error
{
public:
	using runtime_error::runtime_error;
};

class OpDecodeError : public std::runtime_error
{
public:
	using runtime_error::runtime_error;
};

}
