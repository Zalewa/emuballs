#define BOOST_TEST_MODULE machine_armopcode
#include <boost/test/unit_test.hpp>
#include "src/machine/armopcode.hpp"
#include "src/machine/armmachine.hpp"

using namespace Machine;

class FakeCode : public ArmOpCode
{
public:
	bool wasRun;

	FakeCode(uint32_t code) : ArmOpCode(code << 28)
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

static bool execCode(uint32_t conditional, uint32_t flags)
{
	ArmMachine machine;
	machine.cpu().flags().store(flags);
	FakeCode opcode(conditional);
	opcode.execute(machine);
	return opcode.wasRun;
}

constexpr static auto allflags = 0xf0000000UL;
constexpr static auto noflags = 0UL;

BOOST_FIXTURE_TEST_SUITE(machine_armopcode, Fixture)

BOOST_AUTO_TEST_CASE(eq_equal)
{
	constexpr auto code = 0b0000;
	BOOST_CHECK(execCode(code, ArmFlags::F_ZERO));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(!execCode(code, allflags & ~ArmFlags::F_ZERO));
}

BOOST_AUTO_TEST_CASE(ne_not_equal)
{
	constexpr auto code = 0b0001;
	BOOST_CHECK(!execCode(code, ArmFlags::F_ZERO));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(execCode(code, allflags & ~ArmFlags::F_ZERO));
}

BOOST_AUTO_TEST_CASE(cs_unsigned_higer_or_same)
{
	constexpr auto code = 0b0010;
	BOOST_CHECK(execCode(code, ArmFlags::F_CARRY));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(!execCode(code, allflags & ~ArmFlags::F_CARRY));
}

BOOST_AUTO_TEST_CASE(cc_unsigned_lower)
{
	constexpr auto code = 0b0011;
	BOOST_CHECK(!execCode(code, ArmFlags::F_CARRY));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(execCode(code, allflags & ~ArmFlags::F_CARRY));
}

BOOST_AUTO_TEST_CASE(mi_negative)
{
	constexpr auto code = 0b0100;
	BOOST_CHECK(execCode(code, ArmFlags::F_NEGATIVE));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(!execCode(code, allflags & ~ArmFlags::F_NEGATIVE));
}

BOOST_AUTO_TEST_CASE(pl_positive_or_zero)
{
	constexpr auto code = 0b0101;
	BOOST_CHECK(!execCode(code, ArmFlags::F_NEGATIVE));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(execCode(code, allflags & ~ArmFlags::F_NEGATIVE));
}

BOOST_AUTO_TEST_CASE(vs_overflow)
{
	constexpr auto code = 0b0110;
	BOOST_CHECK(execCode(code, ArmFlags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(!execCode(code, allflags & ~ArmFlags::F_OVERFLOW));
}

BOOST_AUTO_TEST_CASE(vc_no_overflow)
{
	constexpr auto code = 0b0111;
	BOOST_CHECK(!execCode(code, ArmFlags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(execCode(code, allflags & ~ArmFlags::F_OVERFLOW));
}

BOOST_AUTO_TEST_CASE(hi_unsigned_higher)
{
	constexpr auto code = 0b1000;
	BOOST_CHECK(execCode(code, ArmFlags::F_CARRY));
	BOOST_CHECK(execCode(code, allflags & ~ArmFlags::F_ZERO));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(!execCode(code, allflags & ~(ArmFlags::F_ZERO | ArmFlags::F_CARRY)));
}

BOOST_AUTO_TEST_CASE(ls_unsigned_lower_or_same)
{
	constexpr auto code = 0b1001;
	BOOST_CHECK(execCode(code, allflags & ~ArmFlags::F_CARRY));
	BOOST_CHECK(execCode(code, ArmFlags::F_ZERO));
	BOOST_CHECK(execCode(code, allflags & ~(ArmFlags::F_ZERO | ArmFlags::F_CARRY)));
	BOOST_CHECK(execCode(code, noflags));
	BOOST_CHECK(!execCode(code, ArmFlags::F_CARRY));
}

BOOST_AUTO_TEST_CASE(ge_greater_or_equal)
{
	constexpr auto code = 0b1010;
	BOOST_CHECK(execCode(code, noflags));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(execCode(code, ArmFlags::F_NEGATIVE | ArmFlags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, allflags & ~(ArmFlags::F_NEGATIVE | ArmFlags::F_OVERFLOW)));
	BOOST_CHECK(!execCode(code, ArmFlags::F_NEGATIVE));
	BOOST_CHECK(!execCode(code, ArmFlags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, allflags & ~(ArmFlags::F_NEGATIVE)));
	BOOST_CHECK(!execCode(code, allflags & ~(ArmFlags::F_OVERFLOW)));
}

BOOST_AUTO_TEST_CASE(lt_less_than)
{
	constexpr auto code = 0b1011;
	BOOST_CHECK(!execCode(code, noflags));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(!execCode(code, ArmFlags::F_NEGATIVE | ArmFlags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, allflags & ~(ArmFlags::F_NEGATIVE | ArmFlags::F_OVERFLOW)));
	BOOST_CHECK(execCode(code, ArmFlags::F_NEGATIVE));
	BOOST_CHECK(execCode(code, ArmFlags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, allflags & ~(ArmFlags::F_NEGATIVE)));
	BOOST_CHECK(execCode(code, allflags & ~(ArmFlags::F_OVERFLOW)));
}

BOOST_AUTO_TEST_CASE(gt_greater_than)
{
	constexpr auto code = 0b1100;
	BOOST_CHECK(execCode(code, noflags));
	BOOST_CHECK(execCode(code, allflags & ~ArmFlags::F_ZERO));
	BOOST_CHECK(execCode(code, ArmFlags::F_NEGATIVE | ArmFlags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, allflags & ~(ArmFlags::F_ZERO | ArmFlags::F_NEGATIVE | ArmFlags::F_OVERFLOW)));
	BOOST_CHECK(!execCode(code, ArmFlags::F_ZERO));
	BOOST_CHECK(!execCode(code, ArmFlags::F_NEGATIVE));
	BOOST_CHECK(!execCode(code, ArmFlags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, ArmFlags::F_ZERO | ArmFlags::F_NEGATIVE | ArmFlags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, allflags & ~(ArmFlags::F_ZERO | ArmFlags::F_NEGATIVE)));
	BOOST_CHECK(!execCode(code, allflags & ~(ArmFlags::F_ZERO | ArmFlags::F_OVERFLOW)));
}

BOOST_AUTO_TEST_CASE(le_less_than_or_equal)
{
	constexpr auto code = 0b1101;
	BOOST_CHECK(!execCode(code, noflags));
	BOOST_CHECK(!execCode(code, allflags & ~ArmFlags::F_ZERO));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(!execCode(code, ArmFlags::F_NEGATIVE | ArmFlags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, allflags & ~(ArmFlags::F_ZERO | ArmFlags::F_NEGATIVE
		| ArmFlags::F_OVERFLOW)));
	BOOST_CHECK(execCode(code, ArmFlags::F_ZERO));
	BOOST_CHECK(execCode(code, ArmFlags::F_NEGATIVE));
	BOOST_CHECK(execCode(code, ArmFlags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, ArmFlags::F_ZERO | ArmFlags::F_NEGATIVE | ArmFlags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, allflags & ~(ArmFlags::F_ZERO | ArmFlags::F_NEGATIVE)));
	BOOST_CHECK(execCode(code, allflags & ~(ArmFlags::F_ZERO | ArmFlags::F_OVERFLOW)));
}

BOOST_AUTO_TEST_CASE(al_always)
{
	constexpr auto code = 0b1110;
	for (uint32_t i = 0; i < 16; ++i)
	{
		BOOST_CHECK_MESSAGE(execCode(code, i << 28), std::bitset<4>(i).to_string());
	}
}

BOOST_AUTO_TEST_CASE(illegal)
{
	constexpr auto code = 0b1111;
	BOOST_CHECK_THROW(execCode(code, noflags), IllegalOpCodeError);
}

BOOST_AUTO_TEST_SUITE_END()
