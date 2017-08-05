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

OpDecoder::OpDecoder()
{
	currentPage = createPage(currentPageAddress);
}

OpDecoder::OpDecoder(const OpDecoder &other)
{
	std::copy(std::begin(other.decodedOps), std::end(other.decodedOps), this->decodedOps);
	this->opPages = other.opPages;
	this->currentPageAddress = other.currentPageAddress;
	adjustPointers();
}

void swap(OpDecoder &a, OpDecoder &b) noexcept
{
	using std::swap;

	swap(a.decodedOps, b.decodedOps);
	swap(a.opPages, b.opPages);
	swap(a.currentPageAddress, b.currentPageAddress);
	a.adjustPointers();
	b.adjustPointers();
}

void OpDecoder::adjustPointers()
{
	this->currentPage = &this->opPages.find(this->currentPageAddress)->second;
	for (auto it : opPages)
	{
		for (Op &op : it.second)
		{
			if (op.opcode != nullptr)
				op.opcode = findDecodedOp(op.instruction);
		}
	}
}

static OpDecodeError decodeError(const std::string &why, uint32_t code, std::streampos position)
{
	std::stringstream ss;
	ss << why << " " << std::hex << code << " at " << std::dec << position;
	return OpDecodeError(ss.str());
}

Opcode* OpDecoder::next(std::istream &input)
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
	return decode(position, code);
}

Opcode* OpDecoder::decode(memsize address, uint32_t instruction)
{
	// If we're still on the current page, try to find decoded instruction on it.
	if ((address & OP_PAGE_MASK) == currentPageAddress)
	{
		Opcode* ptr = findOpcodeOnCurrentPage(address, instruction);
		if (ptr != nullptr)
			return ptr;
	}
	else
	{
		// If we're not on the same page anymore, try to see if we have a page.
		currentPageAddress = address & OP_PAGE_MASK;
		auto opPageIt = opPages.find(currentPageAddress);
		if (opPageIt != opPages.end())
		{
			currentPage = &opPageIt->second;
			Opcode* ptr = findOpcodeOnCurrentPage(address, instruction);
			if (ptr != nullptr)
				return ptr;
		}
		else
		{
			currentPage = createPage(currentPageAddress);
		}
	}

	// The instruction is either from a new page or self-modifying code
	// changed the instructions. We need to decode a new.

	// Try to find cached opcode - revalidation is not needed.
	Opcode *decodedOp = findDecodedOp(instruction);
	if (decodedOp != nullptr)
	{
		saveOpcodeOnCurrentPage(address, instruction, decodedOp);
		return decodedOp;
	}
	// Try to decode using one of the factories.
	OpcodePtr opcode = nullptr;
	for (auto factory : factories)
	{
		opcode = factory(instruction);
		if (opcode)
		{
			opcode->validate();
			decodedOps[decodedMapAddress(instruction)].insert(std::make_pair(instruction, opcode));
			saveOpcodeOnCurrentPage(address, instruction, opcode.get());
			return opcode.get();
		}
	}
	// Handle bad opcode.
	throw decodeError("unkown opcode", instruction, address);
}

}}
