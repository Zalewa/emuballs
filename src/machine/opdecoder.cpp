#include "opdecoder.hpp"

using namespace Machine;

OpCode *ArmOpDecoder::next(std::istream &input)
{
	uint32_t code = 0;
	input.read(reinterpret_cast<char*>(&code), sizeof(uint32_t));

	// TODO Recognize opcode.

	// TODO Return opcode.

	// TODO Throw error on unrecognized opcode.

	return NULL;
}
