#include "armopcode.hpp"

namespace Machine
{

namespace ArmOpcodes
{

class DataProcessingPsrTransfer : public ArmOpcode
{
};

class Multiply : public ArmOpcode
{
};

class MultiplyLong : public ArmOpcode
{
};

class SingleDataSwap : public ArmOpcode
{
};

class BranchAndExchange : public ArmOpcode
{
};

class HalfwordDataTransferRegisterOffset  : public ArmOpcode
{
};

class HalfwordDataTransferImmediateOffset : public ArmOpcode
{
};

class SingleDataTransfer : public ArmOpcode
{
};

class BlockDataTransfer : public ArmOpcode
{
};

class Branch : public ArmOpcode
{
};

class CoprocessorDataTransfer : public ArmOpcode
{
};

class CoprocessorDataOperation : public ArmOpcode
{
};

class CoprocessorRegisterTransfer : public ArmOpcode
{
};

class SoftwareInterrupt : public ArmOpcode
{
};

}

using namespace ArmOpcodes;

ArmOpcodePtr opcodeDataProcessingPsrTransfer(uint32_t code)
{
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
