#include "armopcode.hpp"

namespace Machine
{

namespace ArmOpcodes
{

class DataProcessingPsrTransfer : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class Multiply : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class MultiplyLong : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class SingleDataSwap : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class BranchAndExchange : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class HalfwordDataTransferRegisterOffset  : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class HalfwordDataTransferImmediateOffset : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class DoublewordDataTransfer  : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class SingleDataTransfer : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class BlockDataTransfer : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class Branch : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class CoprocessorDataTransfer : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class CoprocessorDataOperation : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class CoprocessorRegisterTransfer : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

class SoftwareInterrupt : public ArmOpcode
{
public:
	using ArmOpcode::ArmOpcode;
protected:
	void run()
	{
	}
};

}

using namespace ArmOpcodes;

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
	return (code & 0x0e1000d0) == 0x000000d0;
}

static bool isMultiply(uint32_t code)
{
	return (code & 0x0fc000f0) == 0x00000090;
}

static bool isMultiplyLong(uint32_t code)
{
	return (code & 0x0f8000f0) == 0x00800090;
}

ArmOpcodePtr opcodeDataProcessingPsrTransfer(uint32_t code)
{
	if (isDataProcessingPsrTransfer(code))
	{
		return ArmOpcodePtr(new DataProcessingPsrTransfer(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeMultiply(uint32_t code)
{
	if (isMultiply(code))
	{
		return ArmOpcodePtr(new Multiply(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeMultiplyLong(uint32_t code)
{
	if (isMultiplyLong(code))
	{
		return ArmOpcodePtr(new MultiplyLong(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeSingleDataSwap(uint32_t code)
{
	if (isSingleDataSwap(code))
	{
		return ArmOpcodePtr(new SingleDataSwap(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeBranchAndExchange(uint32_t code)
{
	if (isBxMagic(code))
	{
		return ArmOpcodePtr(new BranchAndExchange(code));
	}
	return nullptr;
}

static bool canBeHalfwordDataTransfer(uint32_t code)
{
	return !(isSingleDataSwap(code) || isDoublewordDataTransfer(code) ||
		isMultiply(code) || isMultiplyLong(code));
}

ArmOpcodePtr opcodeHalfwordDataTransferRegisterOffset(uint32_t code)
{
	if (canBeHalfwordDataTransfer(code) && (code & 0x0e400f90) == 0x00000090)
	{
		return ArmOpcodePtr(new HalfwordDataTransferRegisterOffset(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeHalfwordDataTransferImmediateOffset(uint32_t code)
{
	if (canBeHalfwordDataTransfer(code) && (code & 0x0e400090) == 0x00400090)
	{
		return ArmOpcodePtr(new HalfwordDataTransferRegisterOffset(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeDoublewordDataTransfer(uint32_t code)
{
	if (isDoublewordDataTransfer(code))
	{
		return ArmOpcodePtr(new DoublewordDataTransfer(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeSingleDataTransfer(uint32_t code)
{
	if ((code & 0x0e000010) == 0x06000010)
	{
		return nullptr;
	}
	if ((code & 0x0c000000) == 0x04000000)
	{
		return ArmOpcodePtr(new SingleDataTransfer(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeBlockDataTransfer(uint32_t code)
{
	if ((code & 0x0e000000) == 0x08000000)
	{
		return ArmOpcodePtr(new BlockDataTransfer(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeBranch(uint32_t code)
{
	if ((code & 0x0e000000) == 0x0a000000)
	{
		return ArmOpcodePtr(new Branch(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeCoprocessorDataTransfer(uint32_t code)
{
	if (((code & 0x0e000000) == 0x0c000000) &&
		((code & 0x0d600000) != 0x0c400000))
	{
		return ArmOpcodePtr(new CoprocessorDataTransfer(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeCoprocessorDataOperation(uint32_t code)
{
	if ((code & 0x0f000010) == 0x0e000000)
	{
		return ArmOpcodePtr(new CoprocessorDataOperation(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeCoprocessorRegisterTransfer(uint32_t code)
{
	if (((code & 0x0f000010) == 0x0e000010) ||
		((code & 0x0d600000) == 0x0c400000))
	{
		return ArmOpcodePtr(new CoprocessorRegisterTransfer(code));
	}
	return nullptr;
}

ArmOpcodePtr opcodeSoftwareInterrupt(uint32_t code)
{
	if ((code & 0x0f000000) == 0x0f000000)
	{
		return ArmOpcodePtr(new SoftwareInterrupt(code));
	}
	return nullptr;
}


}
