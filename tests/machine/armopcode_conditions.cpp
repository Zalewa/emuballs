#define BOOST_TEST_MODULE machine_armopcode
#include <boost/test/unit_test.hpp>
#include "src/machine/armopcode.hpp"
#include "src/machine/armmachine.hpp"

using namespace Machine;

class FakeCode : public ArmOpCode
{
public:
	bool wasRun;

	FakeCode(uint32_t code) : ArmOpCode(code)
	{
		wasRun = false;
	}

	void run()
	{
		wasRun = true;
	}
};

struct Fixture
{
	ArmMachine machine;
};

BOOST_FIXTURE_TEST_SUITE(machine_armopcode, Fixture)

BOOST_AUTO_TEST_CASE(al)
{
	FakeCode opcode(0b1110UL << 28);
	opcode.execute(machine);
	BOOST_CHECK(opcode.wasRun);
}

BOOST_AUTO_TEST_SUITE_END()
