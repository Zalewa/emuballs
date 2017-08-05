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
	this->currentPage = &this->opPages.find(this->currentPageAddress)->second;
}

OpDecoder::OpDecoder(OpDecoder && other) noexcept
{
	swap(*this, other);
}

OpDecoder &OpDecoder::operator=(OpDecoder other)
{
	swap(*this, other);
	return *this;
}

void swap(OpDecoder &a, OpDecoder &b) noexcept
{
	using std::swap;

	swap(a.decodedOps, b.decodedOps);
	swap(a.opPages, b.opPages);
	swap(a.currentPageAddress, b.currentPageAddress);
	a.currentPage = &a.opPages.find(a.currentPageAddress)->second;
	b.currentPage = &b.opPages.find(b.currentPageAddress)->second;
}

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
	return decode(position, code);
}

OpcodePtr OpDecoder::decode(memsize address, uint32_t instruction)
{
	// If we're still on the current page, try to find decoded instruction on it.
	if ((address & OP_PAGE_MASK) == currentPageAddress)
	{
		OpcodePtr ptr = findOpcodeOnCurrentPage(address, instruction);
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
			OpcodePtr ptr = findOpcodeOnCurrentPage(address, instruction);
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
	uint32_t decodedMapAddress = (instruction >> OP_ARRAY_SHIFT) & OP_ARRAY_MASK;
	auto &decodedMap = decodedOps[decodedMapAddress];
	auto cachedOpIt = decodedMap.find(instruction);
	if (cachedOpIt != decodedMap.end())
	{
		saveOpcodeOnCurrentPage(address, instruction, cachedOpIt->second);
		return cachedOpIt->second;
	}
	// Try to decode using one of the factories.
	OpcodePtr opcode = nullptr;
	for (auto factory : factories)
	{
		opcode = factory(instruction);
		if (opcode)
		{
			opcode->validate();
			decodedOps[decodedMapAddress].insert(std::make_pair(instruction, opcode));
			saveOpcodeOnCurrentPage(address, instruction, cachedOpIt->second);
			return opcode;
		}
	}
	// Handle bad opcode.
	throw decodeError("unkown opcode", instruction, address);
}

}}
