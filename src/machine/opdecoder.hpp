#pragma once

#include <istream>

namespace Machine
{

class ArmOpCode;

class ArmOpDecoder
{
public:
	ArmOpCode *next(std::istream &);
};

}
