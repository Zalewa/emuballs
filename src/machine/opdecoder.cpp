#include "opdecoder.hpp"

#include "armopcode_impl.hpp"

#include <sstream>

using namespace Machine::Arm;

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
