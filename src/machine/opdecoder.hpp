#pragma once

#include <istream>

namespace Machine
{

class ArmOpcode;

class ArmOpDecoder
{
public:
	ArmOpcode *next(std::istream &);
};

}
