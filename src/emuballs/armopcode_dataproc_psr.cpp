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
#include "armopcode_dataproc_psr.hpp"

#include "armmachine.hpp"
#include "errors_private.hpp"
#include "shift.hpp"

#define OpLogic(name) class name : public DataProcessingLogic { \
	public: using DataProcessingLogic::DataProcessingLogic; \
	protected: regval calculate(); }; \
	regval name::calculate()

#define OpLogicTest(name) class name : public DataProcessingLogicTest { \
	public: using DataProcessingLogicTest::DataProcessingLogicTest; \
	protected: regval calculate(); }; \
	regval name::calculate()

#define OpArithmetic(name) class name : public DataProcessingArithmetic { \
	public: using DataProcessingArithmetic::DataProcessingArithmetic; \
	protected: regval calculate(); }; \
	regval name::calculate()

#define OpArithmeticTest(name) class name : public DataProcessingArithmeticTest { \
	public: using DataProcessingArithmeticTest::DataProcessingArithmeticTest; \
	protected: regval calculate(); }; \
	regval name::calculate()


namespace Emuballs
{

namespace Arm
{

namespace OpcodeImpl
{

class DataProcessing : public Opcode
{
public:
	DataProcessing(uint32_t code)
		: Opcode(code)
	{
		rn = (code >> 16) & 0xf;
		rd = (code >> 12) & 0xf;
		immediate = code & (1 << 25);
		condition = code & (1 << 20);

		if (immediate)
		{
			precalculatedImmediateOp2 = operand2Immediate();
		}
		else
		{
			rm = code & 0xf;
			shiftType = static_cast<ShiftType>((code >> 5) & 0b11);
			shiftFunction = shifter<uint32_t>(static_cast<int>(shiftType));
			useRegisterToShift = code & (1 << 4);
			if (useRegisterToShift)
				rs = (code >> 8) & 0xf;
			else
				immediateShiftAmount = (code >> 7) & 0x1f;
		}
	}

protected:
	// Immutable attributes.
	int rn;
	int rd;
	bool condition;
	bool immediate;

	// Machine-state dependant attributes.
	regval rnVal;
	uint32_t op2;
	bool shiftCarry;
	bool overflow;

	virtual void validate() override
	{
		if (useRegisterToShift)
		{
			if (rs == 15)
				throw IllegalOpcodeError("shift register cannot be r15");
		}
	}

	virtual void run(Machine &machine)
	{
		auto &cpu = machine.cpu();
		shiftCarry = cpu.flags().carry();
		overflow = cpu.flags().overflow();
		rnVal = cpu.regs()[rn];
		calcOperand2(machine);
		auto endval = calculate();
		if (writeRd())
			cpu.regs().set(rd, endval);
		if (condition)
			adjustFlags(machine, endval);
	}

	virtual regval calculate() = 0;
	virtual void adjustFlags(Machine &machine, regval endval) = 0;
	virtual bool writeRd() const
	{
		return true;
	}

private:
	// Immediate operand 2.
	uint32_t precalculatedImmediateOp2;

	// Register operand 2.
	bool useRegisterToShift;
	int rm;
	int rs;
	int immediateShiftAmount;
	ShiftType shiftType;
	bitshift<uint32_t> shiftFunction;

	void calcOperand2(Machine &machine)
	{
		if (immediate)
		{
			op2 = precalculatedImmediateOp2;
		}
		else
		{
			op2 = operand2ByRegisterShift(machine);
		}
	}

	uint32_t operand2Immediate()
	{
		uint32_t imm = code() & 0xff;
		auto rotate = (code() >> 8) & 0xf;
		return rotateRight<uint32_t>(imm, rotate * 2);
	}

	uint32_t operand2ByRegisterShift(Machine &machine)
	{
		regval value = machine.cpu().regs()[rm];
		int shiftAmount = 0;
		if (useRegisterToShift)
		{
			shiftAmount = machine.cpu().regs()[rs] & 0xff;
		}
		else
		{
			shiftAmount = immediateShiftAmount;
			if (shiftType == ShiftType::ror && shiftAmount == 0)
			{
				return rotateRightExtended(value,
					machine.cpu().flags().carry(),
					&shiftCarry);
			}
			if ((shiftType == ShiftType::lsr || shiftType == ShiftType::asr) && shiftAmount == 0)
				shiftAmount = 32;
		}
		return shiftFunction(value, shiftAmount, &shiftCarry);
	}
};

class DataProcessingLogic : public DataProcessing
{
public:
	using DataProcessing::DataProcessing;

protected:
	void adjustFlags(Machine &machine, regval endval)
	{
		auto &flags = machine.cpu().flags();
		flags.carry(shiftCarry);
		flags.zero(endval == 0);
		flags.negative(endval & (1 << 31));
	}
};

class DataProcessingLogicTest : public DataProcessingLogic
{
public:
	using DataProcessingLogic::DataProcessingLogic;

protected:
	bool writeRd() const override
	{
		return false;
	}
};

class DataProcessingArithmetic : public DataProcessing
{
public:
	using DataProcessing::DataProcessing;

protected:
	bool carryIn;
	bool carryOut;

	void run(Machine &machine)
	{
		carryIn = machine.cpu().flags().carry();
		carryOut = carryIn;
		DataProcessing::run(machine);
	}

	void adjustFlags(Machine &machine, regval endval)
	{
		auto &flags = machine.cpu().flags();
		flags.overflow(overflow);
		flags.carry(carryOut);
		flags.zero(endval == 0);
		flags.negative(endval & (1 << 31));
	}

	void captureFlagsAdd(uint32_t a, uint32_t b)
	{
		carryOut = (a > REGVAL_MAX - b);
		auto sameHighBit = (a & REGVAL_HIGHBIT) == (b & REGVAL_HIGHBIT);
		overflow = sameHighBit && (a & REGVAL_HIGHBIT) != ((a + b) & REGVAL_HIGHBIT);
	}

	void captureFlagsSub(regval minuend, regval subtrahend)
	{
		carryOut = minuend >= subtrahend;
		overflow = (minuend & REGVAL_HIGHBIT) != (subtrahend & REGVAL_HIGHBIT);
		regval subtrahendNegated = ~subtrahend;
		auto sameHighBit = (minuend & REGVAL_HIGHBIT) == (subtrahendNegated & REGVAL_HIGHBIT);
		overflow = sameHighBit &&
			(minuend & REGVAL_HIGHBIT) != ((minuend + subtrahendNegated) & REGVAL_HIGHBIT);
	}
};

class DataProcessingArithmeticTest : public DataProcessingArithmetic
{
public:
	using DataProcessingArithmetic::DataProcessingArithmetic;

protected:
	bool writeRd() const override
	{
		return false;
	}
};


OpLogic(And)
{
	return rnVal & op2;
}

OpLogic(Eor)
{
	return rnVal ^ op2;
}

OpArithmetic(Sub)
{
	captureFlagsSub(rnVal, op2);
	carryOut = rnVal >= op2;
	return rnVal - op2;
}

OpArithmetic(Rsb)
{
	captureFlagsSub(op2, rnVal);
	return op2 - rnVal;
}

OpArithmetic(Add)
{
	captureFlagsAdd(rnVal, op2);
	return rnVal + op2;
}

OpArithmetic(Adc)
{
	auto carried = carryIn ? 1 : 0;
	captureFlagsAdd(rnVal, op2);
	return rnVal + op2 + carried;
}

OpArithmetic(Sbc)
{
	auto carried = carryIn ? 0 : 1;
	captureFlagsSub(rnVal, op2);
	return (rnVal - op2) - carried;
}

OpArithmetic(Rsc)
{
	auto carried = carryIn ? 0 : 1;
	captureFlagsSub(op2, rnVal);
	return (op2 - rnVal) - carried;
}

OpLogicTest(Tst)
{
	return rnVal & op2;
}

OpLogicTest(Teq)
{
	return rnVal ^ op2;
}

OpArithmeticTest(Cmp)
{
	captureFlagsSub(rnVal, op2);
	return rnVal - op2;
}

OpArithmeticTest(Cmn)
{
	captureFlagsAdd(rnVal, op2);
	return rnVal + op2;
}

OpLogic(Orr)
{
	return rnVal | op2;
}

OpLogic(Mov)
{
	return op2;
}

OpLogic(Bic)
{
	return rnVal & ~op2;
}

OpLogic(Mvn)
{
	return ~op2;
}

class PsrOp : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	Flags &psr(Machine &machine)
	{
		return code() & (1 << 22) ? machine.cpu().flagsSpsr() : machine.cpu().flags();
	}
};

class Mrs : public PsrOp
{
public:
	using PsrOp::PsrOp;
protected:
	void run(Machine &machine)
	{
		auto rd = (code() >> 12) & 0xf;
		auto &flags = psr(machine);
		machine.cpu().regs().set(rd, flags.dump());
	}
};

class Msr : public PsrOp
{
public:
	using PsrOp::PsrOp;
protected:
	void run(Machine &machine)
	{
		bool immediate = (code() >> 25) & 1;
		auto &flags = psr(machine);
		regval value;
		if (immediate)
		{
			value = rotateRight((code() & 0xff), ((code() >> 8) & 0xf) * 2);
		}
		else
		{
			auto rm = code() & 0xf;
			value = machine.cpu().regs()[rm];
		}
		auto sections = (code() >> 16) & 0xf;
		auto mask = 0;
		if (sections & control_bits)
			mask |= 0xff;
		if (sections & extension_bits)
			mask |= 0xff00;
		if (sections & status_bits)
			mask |= 0xff0000;
		if (sections & flag_bits)
			mask |= 0xff000000;
		regval currentFlags = flags.dump() & ~mask;
		regval newFlags = currentFlags | (value & mask);
		flags.store(newFlags);
	}

private:
	constexpr static auto control_bits = 1;
	constexpr static auto extension_bits = 2;
	constexpr static auto status_bits = 4;
	constexpr static auto flag_bits = 8;
};


OpcodePtr decodeDataProcessingPsrTransfer(uint32_t code)
{
	bool condition = code & (1 << 20);
	auto opCode = (code >> 21) & 0xf;
	switch (opCode)
	{
	case 0:
		return OpcodePtr(new And(code));
	case 1:
		return OpcodePtr(new Eor(code));
	case 2:
		return OpcodePtr(new Sub(code));
	case 3:
		return OpcodePtr(new Rsb(code));
	case 4:
		return OpcodePtr(new Add(code));
	case 5:
		return OpcodePtr(new Adc(code));
	case 6:
		return OpcodePtr(new Sbc(code));
	case 7:
		return OpcodePtr(new Rsc(code));
	case 8:
		if (condition)
			return OpcodePtr(new Tst(code));
		else
			return OpcodePtr(new Mrs(code));
	case 9:
		if (condition)
			return OpcodePtr(new Teq(code));
		else
			return OpcodePtr(new Msr(code));
	case 10:
		if (condition)
			return OpcodePtr(new Cmp(code));
		else
			return OpcodePtr(new Mrs(code));
	case 11:
		if (condition)
			return OpcodePtr(new Cmn(code));
		else
			return OpcodePtr(new Msr(code));
	case 12:
		return OpcodePtr(new Orr(code));
	case 13:
		return OpcodePtr(new Mov(code));
	case 14:
		return OpcodePtr(new Bic(code));
	case 15:
		return OpcodePtr(new Mvn(code));
	default:
		throw UnhandledCaseError("data processing op code "
			+ std::to_string(opCode)  + " is unhandled");
	}
}

}

}

}
