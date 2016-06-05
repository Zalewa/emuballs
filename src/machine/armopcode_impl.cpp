#include "armopcode.hpp"

namespace Machine
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
	void run()
	{
	}
};

class Multiply : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class MultiplyLong : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class SingleDataSwap : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class BranchAndExchange : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class HalfwordDataTransferRegisterOffset  : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class HalfwordDataTransferImmediateOffset : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class DoublewordDataTransfer  : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class SingleDataTransfer : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class BlockDataTransfer : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class Branch : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class CoprocessorDataTransfer : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class CoprocessorDataOperation : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class CoprocessorRegisterTransfer : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
	{
	}
};

class SoftwareInterrupt : public Opcode
{
public:
	using Opcode::Opcode;
protected:
	void run()
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

} // namespace 

} // namespace Machine
