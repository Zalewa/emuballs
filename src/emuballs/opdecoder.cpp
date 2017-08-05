/*
 * Copyright 2016 Zalewa <zalewapl@gmail.com>.
 *
 * This file is part of Emuballs.
 *
 * Emuballs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Emuballs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Emuballs.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "opdecoder.hpp"

#include "emuballs/errors.hpp"

#include "armopcode_impl.hpp"

#include <sstream>

namespace Emuballs { namespace Arm {

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
	#ifdef EMUBALLS_BIG_ENDIAN
	#error("big endian not supported")
	#endif
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
	// Try to find cached opcode - revalidation is not needed.
	auto cachedOpIt = decodedOps.find(instruction);
	if (cachedOpIt != decodedOps.end())
	{
		opcode = cachedOpIt->second;
	}
	// Try to decode using one of the factories.
	if (!opcode)
	{
		for (auto factory : factories)
		{
			opcode = factory(instruction);
			if (opcode)
			{
				opcode->validate();
				decodedOps.insert(std::make_pair(instruction, opcode));
				break;
			}
		}
	}
	// Handle bad opcode.
	if (!opcode)
	{
		throw decodeError("unkown opcode", instruction, 0);
	}

	return opcode;
}

}}
