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
#include "armopcode.hpp"

#include "armmachine.hpp"
#include "armopcode_dataproc_psr.hpp"
#include "errors.hpp"
#include "memory.hpp"
#include "shift.hpp"
#include <algorithm>

namespace Emuballs
{

namespace Arm
{

namespace OpcodeImpl
{

class DataProcessingPsrTransfer : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run(Machine &machine)
	{
		OpcodePtr ptr = decodeDataProcessingPsrTransfer(code());
		ptr->execute(machine);
	}
};

class Multiply : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void validate()
	{
		auto opregs = {rd(), rn(), rs(), rm()};
		if (std::any_of(begin(opregs), end(opregs), [](auto reg){return reg == 15;}))
			throw IllegalOpcodeError("mul: register musn't be r15");
		if (rd() == rm())
			throw IllegalOpcodeError("mul: rd musn't be same as rm");
	}

	void run(Machine &machine)
	{
		bool condition = code() & (1 << 20);
		bool accumulate = code() & (1 << 21);
		auto &regs = machine.cpu().regs();
		regs[rd()] = regs[rs()] * regs[rm()];
		if (accumulate)
		{
			regs[rd()] += regs[rn()];
		}
		if (condition)
		{
			auto &flags = machine.cpu().flags();
			flags.set(Flags::Zero, regs[rd()] == 0);
			flags.set(Flags::Negative, regs[rd()] & (1 << 31));
			// "The C (Carry) flag is set to a meaningless value"
			// ~ ARM7TDMI Data Sheet
			flags.set(Flags::Carry, regs[rd()] & 0x4);
		}
	}

private:
	int rd() const
	{
		return (code() >> 16) & 0xf;
	}

	int rn() const
	{
		return (code() >> 12) & 0xf;
	}

	int rs() const
	{
		return (code() >> 8) & 0xf;
	}

	int rm() const
	{
		return code() & 0xf;
	}

};

class MultiplyLong : public Opcode
{
public:
	using Opcode::Opcode;

protected:
	void validate()
	{
		auto regs = { rs(), rm(), rdHi(), rdLo() };
		if (std::any_of(begin(regs), end(regs), [](auto reg){return reg == 15;}))
			throw IllegalOpcodeError("mull: register cannot be 15");
		if (rdHi() == rdLo() || rdHi() == rm() || rdLo() == rm())
			throw IllegalOpcodeError("mull: registers rdHi, rdLo and rm must be different");
	}

	void run(Machine &machine)
	{
		bool condition = code() & (1 << 20);
		bool accumulate = code() & (1 << 21);
		bool signedOperands = code() & (1 << 22);
		auto &regs = machine.cpu().regs();
		uint64_t endResult = 0;
		if (signedOperands)
		{
			int64_t accumulated = 0;
			if (accumulate)
			{
				accumulated |= static_cast<uint64_t>(regs[rdHi()]) << 32;
				accumulated |= regs[rdLo()];
			}
			int64_t a = static_cast<int32_t>(regs[rm()]);
			int64_t b = static_cast<int32_t>(regs[rs()]);
			endResult = static_cast<uint64_t>(accumulated + (a * b));
		}
		else
		{
			uint64_t accumulated = 0;
			if (accumulate)
			{
				accumulated |= static_cast<uint64_t>(regs[rdHi()]) << 32;
				accumulated |= regs[rdLo()];
			}
			uint64_t a = regs[rm()];
			uint64_t b = regs[rs()];
			endResult = static_cast<uint64_t>(accumulated + (a * b));
		}
		regs[rdLo()] = static_cast<uint32_t>(endResult & 0xffffffff);
		regs[rdHi()] = static_cast<uint32_t>((endResult >> 32) & 0xffffffff);
		if (condition)
		{
			auto &flags = machine.cpu().flags();
			flags.set(Flags::Zero, endResult == 0);
			flags.set(Flags::Negative, endResult & (1ULL << 63));
			// "Both the C and V flags are set to meaningless values."
			// ~ ARM7TDMI Data Sheet
			flags.set(Flags::Carry, regs[rdLo()] & 0x4);
			flags.set(Flags::Overflow, regs[rdLo()] & 0x400);
		}
	}

private:
	int rs() const
	{
		return (code() >> 8) & 0xf;
	}

	int rm() const
	{
		return code() & 0xf;
	}

	int rdHi() const
	{
		return (code() >> 16) & 0xf;
	}

	int rdLo() const
	{
		return (code() >> 12) & 0xf;
	}
};

class SingleDataSwap : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void validate()
	{
		if (rd() == 15 || rm() == 15 || rn() == 15)
		{
			throw IllegalOpcodeError("swp: register musn't be r15");
		}
	}

	void run(Machine &machine)
	{
		bool transferByte = code() & (1 << 22);
		auto rn = this->rn();
		auto rd = this->rd();
		auto rm = this->rm();
		auto addr = machine.cpu().regs()[rn];
		if (transferByte)
		{
			regval val = machine.memory().byte(addr);
			machine.memory().putByte(addr, machine.cpu().regs()[rm] & 0xff);
			machine.cpu().regs()[rd] = val;
		}
		else
		{
			regval val = machine.memory().word(addr);
			machine.memory().putWord(addr, machine.cpu().regs()[rm]);
			machine.cpu().regs()[rd] = val;
		}
	}

private:
	int rn() const
	{
		return (code() >> 16) & 0xf;
	}

	int rd() const
	{
		return (code() >> 12) & 0xf;
	}

	int rm() const
	{
		return code() & 0xf;
	}
};

class BranchAndExchange : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run(Machine &machine)
	{
		auto rn = code() & 0xf;
		auto address = machine.cpu().regs()[rn];
		bool thumb = address & 1;
		// Pipeline flush is performed by the machine itself everytime when 'pc'
		// value changes.
		if (thumb)
		{
			throw IllegalOpcodeError("bx: branching to thumb is not implemented");
		}
		else
		{
			machine.cpu().regs().pc() = address;
		}
	}
};

class HalfwordDataTransferRegisterOffset  : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run(Machine &machine)
	{
	}
};

class HalfwordDataTransferImmediateOffset : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run(Machine &machine)
	{
	}
};

class DoublewordDataTransfer  : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void validate()
	{
		if (isWriteBack() && rn() == 15)
			throw IllegalOpcodeError("ldr/str cannot write back to r15");
		if (isRegisterOffset())
		{
			auto rm = code() & 0xf;
			if (rm == 15)
				throw IllegalOpcodeError("shift register cannot be r15");
		}
		if ((rd() % 2) != 0)
			throw IllegalOpcodeError("ldrd/strd target/source register must be even");
	}

	void run(Machine &machine)
	{
		bool load = ((code() & (1 << 5)) == 0);
		bool positiveOffset = code() & (1 << 23);
		bool preIndexing = code() & (1 << 24);

		uint32_t offset = 0;
		if (isRegisterOffset())
		{
			auto rm = code() & 0xf;
			auto rmVal = machine.cpu().regs()[rm];
			offset = rmVal;
		}
		else
		{
			auto immediateLow = code() & 0xf;
			auto immediateHigh = code() & 0xf00;
			offset = (immediateHigh >> 4) | immediateLow;
		}
		auto address = machine.cpu().regs()[rn()];
		auto addressWithOffset = address + (positiveOffset ? offset : -offset);
		auto memoryAddress = preIndexing ? addressWithOffset : address;
		if (load)
		{
			uint64_t value = machine.memory().dword(memoryAddress);
			machine.cpu().regs()[rd()] = value & 0xffffffff;
			machine.cpu().regs()[rd2()] = (value >> 32) & 0xffffffff;
		}
		else
		{
			uint64_t value = 0;
			value |= machine.cpu().regs()[rd()];
			value |= static_cast<uint64_t>(machine.cpu().regs()[rd2()]) << 32;
			machine.memory().putDword(memoryAddress, value);
		}
		if (isWriteBack())
		{
			machine.cpu().regs()[rn()] = addressWithOffset;
		}
	}

private:
	bool isRegisterOffset() const
	{
		return (code() & (1 << 22)) == 0;
	}

	bool isWriteBack() const
	{
		bool writeBack = code() & (1 << 21);
		bool preIndexing = code() & (1 << 24);
		return writeBack || !preIndexing;
	}

	int rn() const
	{
		return (code() >> 16) & 0xf;
	}

	int rd() const
	{
		return (code() >> 12) & 0xf;
	}

	int rd2() const
	{
		return rd() + 1;
	}
};

class SingleDataTransfer : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void validate()
	{
		if (isWriteBack() && rn() == 15)
			throw IllegalOpcodeError("ldr/str cannot write back to r15");
		if (isRegisterOffset())
		{
			auto rm = code() & 0xf;
			if (rm == 15)
				throw IllegalOpcodeError("shift register cannot be r15");
		}
	}

	void run(Machine &machine)
	{
		bool load = code() & (1 << 20);
		bool transferByte = code() & (1 << 22);
		bool positiveOffset = code() & (1 << 23);
		bool preIndexing = code() & (1 << 24);

		uint32_t offset = 0;
		if (isRegisterOffset())
		{
			auto rm = code() & 0xf;
			auto shiftAmount = (code() >> 7) & 0x1f;
			auto shiftType = (code() >> 5) & 0b11;
			auto rmVal = machine.cpu().regs()[rm];
			offset = shifter<uint32_t>(shiftType)(rmVal, shiftAmount, nullptr);
		}
		else
		{
			offset = code() & 0xfff;
		}
		auto address = machine.cpu().regs()[rn()];
		auto addressWithOffset = address + (positiveOffset ? offset : -offset);
		auto memoryAddress = preIndexing ? addressWithOffset : address;
		if (transferByte)
		{
			if (load)
			{
				uint8_t value = machine.memory().byte(memoryAddress);
				machine.cpu().regs()[rd()] = value;
			}
			else
			{
				uint8_t value = machine.cpu().regs()[rd()];
				machine.memory().putByte(memoryAddress, value);
			}
		}
		else
		{
			if (load)
			{
				uint32_t value = machine.memory().word(memoryAddress);
				machine.cpu().regs()[rd()] = value;
			}
			else
			{
				uint32_t value = machine.cpu().regs()[rd()];
				machine.memory().putWord(memoryAddress, value);
			}
		}
		if (isWriteBack())
		{
			machine.cpu().regs()[rn()] = addressWithOffset;
		}
	}

private:
	bool isRegisterOffset() const
	{
		return code() & (1 << 25);
	}

	bool isWriteBack() const
	{
		bool writeBack = code() & (1 << 21);
		bool preIndexing = code() & (1 << 24);
		return writeBack || !preIndexing;
	}

	int rn() const
	{
		return (code() >> 16) & 0xf;
	}

	int rd() const
	{
		return (code() >> 12) & 0xf;
	}
};

class BlockDataTransfer : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run(Machine &machine)
	{
	}
};

class Branch : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run(Machine &machine)
	{
		bool link = (1 << 24) & code();
		auto masked = 0x00ffffff & code();
		auto sign = 0x00800000 & code();
		auto offset = (masked << 2) | (sign ? 0xfc000000 : 0);
		if (link)
		{
			auto pc = machine.cpu().regs().pc();
			machine.cpu().regs().lr() = pc - PREFETCH_SIZE + INSTRUCTION_SIZE;
		}
		machine.cpu().regs().pc() += offset;
	}
};

class CoprocessorDataTransfer : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run(Machine &machine)
	{
	}
};

class CoprocessorDataOperation : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run(Machine &machine)
	{
	}
};

class CoprocessorRegisterTransfer : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run(Machine &machine)
	{
	}
};

class SoftwareInterrupt : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run(Machine &machine)
	{
	}
};

} // namespace OpcodeImpl

using namespace OpcodeImpl;

static bool isBxMagic(uint32_t code)
{
	return (code & 0x0ffffff0) == 0x012fff10;
}

static bool isDataProcessingPsrTransfer(uint32_t code)
{
	if (isBxMagic(code))
		return false;
	if ((code & (0b111 << 25)) == (0b1 << 25))
		return true;
	if (((code & (0b111 << 25)) == 0) &&
		((code & (0b1001 << 4)) != (0b1001 << 4)))
	{
		return true;
	}
	return false;
}

static bool isSingleDataSwap(uint32_t code)
{
	return (code & 0x0fb00ff0) == 0x01000090;
}

static bool isDoublewordDataTransfer(uint32_t code)
{
	uint32_t masked = (code & 0x0e1000f0);
	// ldrd or strd
	return masked == 0x000000d0 || masked == 0x000000f0;
}

static bool isMultiply(uint32_t code)
{
	return (code & 0x0fc000f0) == 0x00000090;
}

static bool isMultiplyLong(uint32_t code)
{
	return (code & 0x0f8000f0) == 0x00800090;
}

OpcodePtr opcodeDataProcessingPsrTransfer(uint32_t code)
{
	if (isDataProcessingPsrTransfer(code))
	{
		return OpcodePtr(new DataProcessingPsrTransfer(code));
	}
	return nullptr;
}

OpcodePtr opcodeMultiply(uint32_t code)
{
	if (isMultiply(code))
	{
		return OpcodePtr(new Multiply(code));
	}
	return nullptr;
}

OpcodePtr opcodeMultiplyLong(uint32_t code)
{
	if (isMultiplyLong(code))
	{
		return OpcodePtr(new MultiplyLong(code));
	}
	return nullptr;
}

OpcodePtr opcodeSingleDataSwap(uint32_t code)
{
	if (isSingleDataSwap(code))
	{
		return OpcodePtr(new SingleDataSwap(code));
	}
	return nullptr;
}

OpcodePtr opcodeBranchAndExchange(uint32_t code)
{
	if (isBxMagic(code))
	{
		return OpcodePtr(new BranchAndExchange(code));
	}
	return nullptr;
}

static bool canBeHalfwordDataTransfer(uint32_t code)
{
	return !(isSingleDataSwap(code) || isDoublewordDataTransfer(code) ||
		isMultiply(code) || isMultiplyLong(code));
}

OpcodePtr opcodeHalfwordDataTransferRegisterOffset(uint32_t code)
{
	if (canBeHalfwordDataTransfer(code) && (code & 0x0e400f90) == 0x00000090)
	{
		return OpcodePtr(new HalfwordDataTransferRegisterOffset(code));
	}
	return nullptr;
}

OpcodePtr opcodeHalfwordDataTransferImmediateOffset(uint32_t code)
{
	if (canBeHalfwordDataTransfer(code) && (code & 0x0e400090) == 0x00400090)
	{
		return OpcodePtr(new HalfwordDataTransferRegisterOffset(code));
	}
	return nullptr;
}

OpcodePtr opcodeDoublewordDataTransfer(uint32_t code)
{
	if (isDoublewordDataTransfer(code))
	{
		return OpcodePtr(new DoublewordDataTransfer(code));
	}
	return nullptr;
}

OpcodePtr opcodeSingleDataTransfer(uint32_t code)
{
	if ((code & 0x0e000010) == 0x06000010)
	{
		return nullptr;
	}
	if ((code & 0x0c000000) == 0x04000000)
	{
		return OpcodePtr(new SingleDataTransfer(code));
	}
	return nullptr;
}

OpcodePtr opcodeBlockDataTransfer(uint32_t code)
{
	if ((code & 0x0e000000) == 0x08000000)
	{
		return OpcodePtr(new BlockDataTransfer(code));
	}
	return nullptr;
}

OpcodePtr opcodeBranch(uint32_t code)
{
	if ((code & 0x0e000000) == 0x0a000000)
	{
		return OpcodePtr(new Branch(code));
	}
	return nullptr;
}

OpcodePtr opcodeCoprocessorDataTransfer(uint32_t code)
{
	if (((code & 0x0e000000) == 0x0c000000) &&
		((code & 0x0d600000) != 0x0c400000))
	{
		return OpcodePtr(new CoprocessorDataTransfer(code));
	}
	return nullptr;
}

OpcodePtr opcodeCoprocessorDataOperation(uint32_t code)
{
	if ((code & 0x0f000010) == 0x0e000000)
	{
		return OpcodePtr(new CoprocessorDataOperation(code));
	}
	return nullptr;
}

OpcodePtr opcodeCoprocessorRegisterTransfer(uint32_t code)
{
	if (((code & 0x0f000010) == 0x0e000010) ||
		((code & 0x0d600000) == 0x0c400000))
	{
		return OpcodePtr(new CoprocessorRegisterTransfer(code));
	}
	return nullptr;
}

OpcodePtr opcodeSoftwareInterrupt(uint32_t code)
{
	if ((code & 0x0f000000) == 0x0f000000)
	{
		return OpcodePtr(new SoftwareInterrupt(code));
	}
	return nullptr;
}

} // namespace Arm

} // namespace Emuballs
