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
#pragma once

#include "armopcode.hpp"

namespace Emuballs
{

namespace Arm

{

OpcodePtr opcodeDataProcessingPsrTransfer(uint32_t code);
OpcodePtr opcodeMultiply(uint32_t code);
OpcodePtr opcodeMultiplyLong(uint32_t code);
OpcodePtr opcodeSingleDataSwap(uint32_t code);
OpcodePtr opcodeBranchAndExchange(uint32_t code);
OpcodePtr opcodeHalfwordDataTransfer(uint32_t code);
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
	opcodeHalfwordDataTransfer,
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
