#define BOOST_TEST_MODULE machine_armopcode
#include <boost/test/unit_test.hpp>
#include "src/machine/armopcode.h"

class FakeCode : public ArmOpCode
{
public:
	FakeCode(uint32_t code) : ArmOpCode(code)
	{
	}

	void run()
	{
	}
};

BOOST_AUTO_TEST_CASE(al)
{
	FakeCode opcode((0b1110 << 31));
	
}
