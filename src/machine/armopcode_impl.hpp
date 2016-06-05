#pragma once

#include "armopcode.hpp"

namespace Machine
{

namespace Arm

{

ArmOpcodePtr opcodeDataProcessingPsrTransfer(uint32_t code);
ArmOpcodePtr opcodeMultiply(uint32_t code);
ArmOpcodePtr opcodeMultiplyLong(uint32_t code);
ArmOpcodePtr opcodeSingleDataSwap(uint32_t code);
ArmOpcodePtr opcodeBranchAndExchange(uint32_t code);
ArmOpcodePtr opcodeHalfwordDataTransferRegisterOffset(uint32_t code);
ArmOpcodePtr opcodeHalfwordDataTransferImmediateOffset(uint32_t code);
ArmOpcodePtr opcodeDoublewordDataTransfer(uint32_t code);
ArmOpcodePtr opcodeSingleDataTransfer(uint32_t code);
ArmOpcodePtr opcodeBlockDataTransfer(uint32_t code);
ArmOpcodePtr opcodeBranch(uint32_t code);
ArmOpcodePtr opcodeCoprocessorDataTransfer(uint32_t code);
ArmOpcodePtr opcodeCoprocessorDataOperation(uint32_t code);
ArmOpcodePtr opcodeCoprocessorRegisterTransfer(uint32_t code);
ArmOpcodePtr opcodeSoftwareInterrupt(uint32_t code);

const OpFactory factories[] =
{
	opcodeDataProcessingPsrTransfer,
	opcodeMultiply,
	opcodeMultiplyLong,
	opcodeSingleDataSwap,
	opcodeBranchAndExchange,
	opcodeHalfwordDataTransferRegisterOffset,
	opcodeHalfwordDataTransferImmediateOffset,
	opcodeDoublewordDataTransfer,
	opcodeSingleDataTransfer,
	opcodeBlockDataTransfer,
	opcodeBranch,
	opcodeCoprocessorDataTransfer,
	opcodeCoprocessorDataOperation,
	opcodeCoprocessorRegisterTransfer,
	opcodeSoftwareInterrupt
};


}

}
