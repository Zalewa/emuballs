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

class UnhandledCaseError : public std::logic_error
{
public:
	using logic_error::logic_error;
};

}
