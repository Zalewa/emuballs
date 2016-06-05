#include "opdecoder.hpp"

#include "armopcode_impl.hpp"

#include <sstream>

namespace Machine { namespace Arm {

static OpDecodeError decodeError(const std::string &why, uint32_t code, std::streampos position)
{
	std::stringstream ss;
	ss << why << " " << std::hex << code << " at " << std::dec << position;
	return OpDecodeError(ss.str());
}

OpcodePtr OpDecoder::next(std::istream &input)
{
	auto position = input.tellg();
	uint32_t code = 0;
	// TODO FIXME I'll break on big-endian machines:
	input.read(reinterpret_cast<char*>(&code), sizeof(uint32_t));
	if (!input)
	{
		throw std::istream::failure("buffer read failure");
	}
	try
	{
		return decode(code);
	}
	catch (const OpDecodeError &e)
	{
		throw decodeError("unknown opcode", code, position);
	}
}

OpcodePtr OpDecoder::decode(uint32_t instruction)
{
	OpcodePtr opcode = nullptr;
	for (auto factory : factories)
	{
		opcode = factory(instruction);
		if (opcode)
		{
			break;
		}
	}
	if (!opcode)
	{
		throw decodeError("unkown opcode", instruction, 0);
	}

	return opcode;
}

}}
