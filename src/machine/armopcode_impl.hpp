#pragma once

#include "armopcode.hpp"

namespace Machine
{

namespace Arm

{

OpcodePtr opcodeDataProcessingPsrTransfer(uint32_t code);
OpcodePtr opcodeMultiply(uint32_t code);
OpcodePtr opcodeMultiplyLong(uint32_t code);
OpcodePtr opcodeSingleDataSwap(uint32_t code);
OpcodePtr opcodeBranchAndExchange(uint32_t code);
OpcodePtr opcodeHalfwordDataTransferRegisterOffset(uint32_t code);
OpcodePtr opcodeHalfwordDataTransferImmediateOffset(uint32_t code);
OpcodePtr opcodeDoublewordDataTransfer(uint32_t code);
OpcodePtr opcodeSingleDataTransfer(uint32_t code);
OpcodePtr opcodeBlockDataTransfer(uint32_t code);
OpcodePtr opcodeBranch(uint32_t code);
OpcodePtr opcodeCoprocessorDataTransfer(uint32_t code);
OpcodePtr opcodeCoprocessorDataOperation(uint32_t code);
OpcodePtr opcodeCoprocessorRegisterTransfer(uint32_t code);
OpcodePtr opcodeSoftwareInterrupt(uint32_t code);

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
