#pragma once

#include <istream>

namespace Machine
{

class OpCode;

class OpDecoder
{
public:
	virtual ~OpDecoder() = default;

	virtual OpCode *next(std::istream &) = 0;
};

class ArmOpDecoder : public OpDecoder
{
public:
	OpCode *next(std::istream &);
};

}
