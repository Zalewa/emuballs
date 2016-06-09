#pragma once

#include "armopcode.hpp"

namespace Machine
{
namespace Arm
{
namespace OpcodeImpl
{
OpcodePtr decodeDataProcessingPsrTransfer(uint32_t code);
}
}
}
