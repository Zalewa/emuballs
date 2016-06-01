#include "opdecoder.hpp"

#include <sstream>

using namespace Machine;

namespace Machine
{

ArmOpcodePtr opcodeDataProcessingPsrTransfer(uint32_t code);
ArmOpcodePtr opcodeMultiply(uint32_t code);
ArmOpcodePtr opcodeMultiplyLong(uint32_t code);
ArmOpcodePtr opcodeSingleDataSwap(uint32_t code);
ArmOpcodePtr opcodeBranchAndExchange(uint32_t code);
ArmOpcodePtr opcodeHalfwordDataTransferRegisterOffset(uint32_t code);
ArmOpcodePtr opcodeHalfwordDataTransferImmediateOffset(uint32_t code);
ArmOpcodePtr opcodeSingleDataTransfer(uint32_t code);
ArmOpcodePtr opcodeBlockDataTransfer(uint32_t code);
ArmOpcodePtr opcodeBranch(uint32_t code);
ArmOpcodePtr opcodeCoprocessorDataTransfer(uint32_t code);
ArmOpcodePtr opcodeCoprocessorDataOperation(uint32_t code);
ArmOpcodePtr opcodeCoprocessorRegisterTransfer(uint32_t code);
ArmOpcodePtr opcodeSoftwareInterrupt(uint32_t code);

ArmOpDecoder::OpFactory factories[] =
{
	opcodeDataProcessingPsrTransfer,
	opcodeMultiply,
	opcodeMultiplyLong,
	opcodeSingleDataSwap,
	opcodeBranchAndExchange,
	opcodeHalfwordDataTransferRegisterOffset,
	opcodeHalfwordDataTransferImmediateOffset,
	opcodeSingleDataTransfer,
	opcodeBlockDataTransfer,
	opcodeBranch,
	opcodeCoprocessorDataTransfer,
	opcodeCoprocessorDataOperation,
	opcodeCoprocessorRegisterTransfer,
	opcodeSoftwareInterrupt
};

}


static void throwDecodeError(const std::string &why, uint32_t code, std::streampos position)
{
	std::stringstream ss;
	ss << why << " " << std::hex << code << " at " << std::dec << position;
	throw OpDecodeError(ss.str());
}

std::unique_ptr<ArmOpcode> ArmOpDecoder::next(std::istream &input)
{
	auto position = input.tellg();
	uint32_t code = 0;
	// TODO FIXME I'll break on big-endian machines:
	input.read(reinterpret_cast<char*>(&code), sizeof(uint32_t));
	if (!input)
	{
		throw std::istream::failure("buffer read failure");
	}

	if ((code & 0xf0000000) == 0xf0000000)
	{
		throwDecodeError("invalid conditional in opcode", code, position);
	}

	std::unique_ptr<ArmOpcode> opcode = nullptr;
	for (auto factory : factories)
	{
		opcode = factory(code);
		if (opcode)
		{
			break;
		}
	}
	if (!opcode)
	{
		throwDecodeError("unkown opcode", code, position);
	}

	return opcode;
}
