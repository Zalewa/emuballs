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

static constexpr auto bx_magic = 0x012fff10;

static bool isDataProcessingPsrTransfer(uint32_t code)
{
	if ((code & bx_magic) == bx_magic)
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
	return nullptr;
}

ArmOpcodePtr opcodeMultiplyLong(uint32_t code)
{
	return nullptr;
}

ArmOpcodePtr opcodeSingleDataSwap(uint32_t code)
{
	return nullptr;
}

ArmOpcodePtr opcodeBranchAndExchange(uint32_t code)
{
	return nullptr;
}

ArmOpcodePtr opcodeHalfwordDataTransferRegisterOffset(uint32_t code)
{
	return nullptr;
}

ArmOpcodePtr opcodeHalfwordDataTransferImmediateOffset(uint32_t code)
{
	return nullptr;
}

ArmOpcodePtr opcodeSingleDataTransfer(uint32_t code)
{
	return nullptr;
}

ArmOpcodePtr opcodeBlockDataTransfer(uint32_t code)
{
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
	return nullptr;
}

ArmOpcodePtr opcodeCoprocessorDataOperation(uint32_t code)
{
	return nullptr;
}

ArmOpcodePtr opcodeCoprocessorRegisterTransfer(uint32_t code)
{
	return nullptr;
}

ArmOpcodePtr opcodeSoftwareInterrupt(uint32_t code)
{
	return nullptr;
}


}
