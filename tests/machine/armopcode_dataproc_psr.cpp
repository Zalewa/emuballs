#define BOOST_TEST_MODULE armopcode_dataproc_psr
#include <boost/test/unit_test.hpp>
#include "src/machine/armopcode_dataproc_psr.hpp"
#include "arm_program_fixture.hpp"

using namespace Machine::Arm::OpcodeImpl;

static auto decode(uint32_t code)
{
	return decodeDataProcessingPsrTransfer(code);
}

BOOST_FIXTURE_TEST_SUITE(armopcode_dataproc_psr, ArmProgramFixture)

BOOST_AUTO_TEST_CASE(and_op)
{
	auto op = decode(0xe0000001); // and	r0, r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);

	r(0, 0xff00ff00);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x0f000f00);
}

BOOST_AUTO_TEST_CASE(eor)
{
	auto op = decode(0xe0200001); // eor	r0, r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xffffffff);

	r(0, 0xff00ff00);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf00ff00f);
}

BOOST_AUTO_TEST_CASE(eor_lsl)
{
	auto op = decode(0xe0200181); // eor	r0, r0, r1, lsl #3
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x88888888);

	r(0, 0xff00ff00);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x87788778);
}

BOOST_AUTO_TEST_CASE(sub)
{
	auto op = decode(0xe0400001); // sub	r0, r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xe1e1e1e1);

	r(0, 30);
	r(1, 11);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 19);

	r(0, 30);
	r(1, -11);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 41);

	r(0, -11);
	r(1, 30);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -41);
}

BOOST_AUTO_TEST_CASE(sub_lsr)
{
	auto op = decode(0xe0400231); // sub	r0, r0, r1, lsr r2
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	r(2, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xe9696969);

	r(0, 30);
	r(1, 110);
	r(2, 4);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 24);

	r(0, 30);
	r(1, -11);
	r(2, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -2147483612);

	r(0, -11);
	r(1, 30);
	r(2, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -41);
}

BOOST_AUTO_TEST_CASE(rsb)
{
	auto op = decode(0xe0600001); // rsb	r0, r0, r1
	r(0, 0x0f0f0f0f);
	r(1, 0xf0f0f0f0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xe1e1e1e1);

	r(0, 11);
	r(1, 30);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 19);

	r(0, -11);
	r(1, 30);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 41);

	r(0, 30);
	r(1, -11);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -41);
}

BOOST_AUTO_TEST_CASE(rsb_asr)
{
	auto op = decode(0xe06000c1); // rsb	r0, r0, r1, asr #1
	r(0, 0x0f0f0f0f);
	r(1, 0xf0f0f0f0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xe9696969);

	r(0, 11);
	r(1, 30);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 4);

	r(0, -11);
	r(1, 30);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 26);

	r(0, 30);
	r(1, -11);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -36);
}

BOOST_AUTO_TEST_CASE(add)
{
	auto op = decode(0xe0800001); // add	r0, r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xffffffff);

	r(0, 24);
	r(1, 12);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 36);

	r(0, 24);
	r(1, -13);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 11);
}

BOOST_AUTO_TEST_CASE(add_ror)
{
	auto op = decode(0xe0800861); // add	r0, r0, r1, ror #16
	r(0, 0xf0f0f0f0);
	r(1, 0x00000f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xfffff0f0);

	r(0, 24);
	r(1, 12);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 786456);

	r(0, 24);
	r(1, -13);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xfff40017);
}

BOOST_AUTO_TEST_CASE(adc)
{
	auto op = decode(0xe0a00001); // adc	r0, r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0e);
	flags().carry(false);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xfffffffe);

	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0e);
	flags().carry(true);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xffffffff);
}

BOOST_AUTO_TEST_CASE(sbc)
{
	auto op = decode(0xe0c00001); // sbc	r0, r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0e);
	flags().carry(false);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xe1e1e1e1);

	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0e);
	flags().carry(true);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xe1e1e1e2);
}

BOOST_AUTO_TEST_CASE(rsc)
{
	auto op = decode(0xe0e00001); // rsc	r0, r0, r1
	r(0, 0x0f0f0f0e);
	r(1, 0xf0f0f0f0);
	flags().carry(false);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xe1e1e1e1);

	r(0, 0x0f0f0f0e);
	r(1, 0xf0f0f0f0);
	flags().carry(true);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xe1e1e1e2);
}

BOOST_AUTO_TEST_CASE(tst)
{
	auto op = decode(0xe1100001); // tst	r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0f0f0f0);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 0xf0404040);
	r(1, 0x8f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0404040);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	r(0, 0x00404040);
	r(1, 0x0f0fff0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x00404040);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());
}

BOOST_AUTO_TEST_CASE(teq)
{
	auto op = decode(0xe1300001); // teq	r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0f0f0f0);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	r(0, 0xf0404040);
	r(1, 0xf0404040);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0404040);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 0x00f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x00f0f0f0);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());
}

BOOST_AUTO_TEST_CASE(cmp)
{
	auto op = decode(0xe1500001); // cmp	r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0f0f0f0);
	BOOST_CHECK(flags().overflow());
	BOOST_CHECK(flags().carry());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	r(0, 0xf0404040);
	r(1, 0xf0404040);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0404040);
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(flags().carry());
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 0x00f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x00f0f0f0);
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(!flags().carry());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());
}

BOOST_AUTO_TEST_CASE(cmn)
{
	auto op = decode(0xe1700001); // cmn	r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0f0f0f0);
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(!flags().carry());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	r(0, 0xf0404040);
	r(1, 0xf0404040);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0404040);
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(flags().carry());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	r(0, 0x00f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x00f0f0f0);
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(!flags().carry());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());
}

BOOST_AUTO_TEST_CASE(orr)
{
	auto op = decode(0xe1800001); // orr	r0, r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xffffffff);

	r(0, 0xff00ff00);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xff0fff0f);
}

BOOST_AUTO_TEST_CASE(mov)
{
	auto op = decode(0xe1a00001); // mov	r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x0f0f0f0f);

	r(0, 0xff00ff00);
	r(1, 0x0f5f3f2f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x0f5f3f2f);
}

BOOST_AUTO_TEST_CASE(bic)
{
	auto op = decode(0xe1c00001); // bic	r0, r0, r1
	r(0, 0xf0f0f0f0);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0f0f0f0);

	r(0, 0xff00ff00);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf000f000);
}

BOOST_AUTO_TEST_CASE(mvn)
{
	auto op = decode(0xe1e00001); // mvn	r0, r1
	r(0, 0xa1b2c3d4);
	r(1, 0x0f0f0f0f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), ~0x0f0f0f0f);

	r(0, 0xff00ff00);
	r(1, 0x0f5f3f2f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), ~0x0f5f3f2f);
}

BOOST_AUTO_TEST_CASE(and_op_imm)
{
	auto op = decode(0xe2000b3d); // and	r0, r0, #62464	; 0xf400
	r(0, 0xf0f0eff0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xe400);
}

BOOST_AUTO_TEST_CASE(eor_imm)
{
	auto op = decode(0xe2200b3d); // eor	r0, r0, #62464	; 0xf400
	r(0, 0xf0f0eff0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0f01bf0);
}

BOOST_AUTO_TEST_CASE(sub_imm)
{
	auto op = decode(0xe2400001); // sub	r0, r0, #1
	r(0, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -1);

	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -2);
}

BOOST_AUTO_TEST_CASE(rsb_imm)
{
	auto op = decode(0xe2600001); // rsb	r0, r0, #1
	r(0, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -1);

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);

	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 2);
}

BOOST_AUTO_TEST_CASE(add_imm)
{
	auto op = decode(0xe2800001); // add	r0, r0, #1
	r(0, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 3);

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 2);

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);

	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
}

BOOST_AUTO_TEST_CASE(adc_imm)
{
	auto op = decode(0xe2a00001); // adc	r0, r0, #1
	flags().carry(false);
	r(0, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 3);

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 2);

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);

	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);

	flags().carry(true);
	r(0, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 4);

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 3);

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 2);

	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);

	r(0, -2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
}

BOOST_AUTO_TEST_CASE(sbc_imm)
{
	auto op = decode(0xe2c00001); // sbc	r0, r0, #1
	flags().carry(true);
	r(0, 5);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 4);

	r(0, 4);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 3);

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -1);

	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -2);

	flags().carry(false);
	r(0, 5);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 3);

	r(0, 4);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 2);

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -1);

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -2);

	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -3);
}

BOOST_AUTO_TEST_CASE(rsc_imm)
{
	auto op = decode(0xe2e00001); // rsc	r0, r0, #1
	flags().carry(true);
	r(0, 5);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -4);

	r(0, 4);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -3);

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);

	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 2);

	flags().carry(false);
	r(0, 5);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -5);

	r(0, 4);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -4);

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -1);

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);

	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);
}

BOOST_AUTO_TEST_CASE(tst_imm)
{
	auto op = decode(0xe3100001); // tst	r0, #1
	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -1);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 2);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 0b11);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0b11);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());
}

BOOST_AUTO_TEST_CASE(teq_imm)
{
	auto op = decode(0xe3300001); // teq	r0, #1
	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -1);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 2);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());
}

BOOST_AUTO_TEST_CASE(cmp_imm)
{
	auto op = decode(0xe3500001); // cmp	r0, #1
	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -1);
	BOOST_CHECK(flags().carry());
	BOOST_CHECK(flags().overflow());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
	BOOST_CHECK(!flags().carry());
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);
	BOOST_CHECK(flags().carry());
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 2);
	BOOST_CHECK(flags().carry());
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());
}

BOOST_AUTO_TEST_CASE(cmn_imm)
{
	auto op = decode(0xe3700001); // cmn	r0, #1
	r(0, -1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), -1);
	BOOST_CHECK(flags().carry());
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
	BOOST_CHECK(!flags().carry());
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);
	BOOST_CHECK(!flags().carry());
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());

	r(0, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 2);
	BOOST_CHECK(!flags().carry());
	BOOST_CHECK(!flags().overflow());
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());
}

BOOST_AUTO_TEST_CASE(orr_imm)
{
	auto op = decode(0xe3800b3d); // orr	r0, r0, #62464	; 0xf400
	r(0, 0xf0f0e3f0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0f0f7f0);
}

BOOST_AUTO_TEST_CASE(mov_imm)
{
	auto op = decode(0xe3a00001); // mov	r0, #1
	r(0, 0xf0f0e3f0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 1);
}

BOOST_AUTO_TEST_CASE(bic_imm)
{
	auto op = decode(0xe3c00b3d); // bic	r0, r0, #62464	; 0xf400
	r(0, 0xf0f0e6f1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xf0f002f1);
}

BOOST_AUTO_TEST_CASE(mvn_imm)
{
	auto op = decode(0xe3e00001); // mvn	r0, #1
	r(0, 0xf0f0e3f0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xfffffffe);
}

BOOST_AUTO_TEST_CASE(mrs_cpsr)
{
	auto op = decode(0xe10f0000); // mrs	r0, CPSR
	flags().store(0xe5b9d7c3);
	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xe5b9d7c3);
}

BOOST_AUTO_TEST_CASE(mrs_spsr)
{
	auto op = decode(0xe14f0000); // mrs	r0, SPSR
	machine.cpu().flags().store(0xcafe0000);
	machine.cpu().flagsSpsr().store(0xd7c3e5b9);
	r(0, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xd7c3e5b9);
}

BOOST_AUTO_TEST_SUITE_END()

struct MsrTestCase
{
	uint32_t instruction;
	std::string psr;
	uint32_t mask;
};

static Machine::Arm::Flags &testCaseFlags(Machine::Arm::Machine &machine, const std::string &psr)
{
	if (psr == "cpsr")
	{
		return machine.cpu().flags();
	}
	else if (psr == "spsr")
	{
		return machine.cpu().flagsSpsr();
	}
	else
	{
		throw std::logic_error("unknown psr " + psr);
	}
}

BOOST_AUTO_TEST_CASE(msr_reg)
{
	MsrTestCase testSet[] = {
		{0xe129f000, "cpsr", 0xff0000ff}, // msr	CPSR_fc, r0
		{0xe169f000, "spsr", 0xff0000ff}, // msr	SPSR_fc, r0
		{0xe128f000, "cpsr", 0xff000000}, // msr	CPSR_f, r0
		{0xe168f000, "spsr", 0xff000000}, // msr	SPSR_f, r0
		{0xe122f000, "cpsr", 0x0000ff00}, // msr	CPSR_x, r0
		{0xe162f000, "spsr", 0x0000ff00}, // msr	SPSR_x, r0
		{0xe121f000, "cpsr", 0x000000ff}, // msr	CPSR_c, r0
		{0xe161f000, "spsr", 0x000000ff}, // msr	SPSR_c, r0
		{0xe124f000, "cpsr", 0x00ff0000}, // msr	CPSR_s, r0
		{0xe164f000, "spsr", 0x00ff0000}, // msr	SPSR_s, r0
		{0xe12ff000, "cpsr", 0xffffffff}, // msr	CPSR_cxsf, r0
		{0xe16ff000, "spsr", 0xffffffff}  // msr	SPSR_cxsf, r0
	};
	auto testVal = 0xffffffff;
	for (auto &testCase : testSet)
	{
		auto op = decode(testCase.instruction);
		ArmProgramFixture fixture;
		auto &flags = testCaseFlags(fixture.machine, testCase.psr);

		flags.store(0);
		fixture.r(0, testVal);
		op->execute(fixture.machine);

		std::stringstream ss;
		ss << std::hex << testCase.instruction << ", " << testCase.psr << ", " << testCase.mask
			<< " -> " << flags.dump() << " == " << (testVal & testCase.mask);
		BOOST_CHECK_MESSAGE(flags.dump() == (testVal & testCase.mask), ss.str());
	}
}

BOOST_AUTO_TEST_CASE(msr_imm)
{
	MsrTestCase testSet[] = {
		{0xe328f4ff, "cpsr", 0xff000000}, // msr	CPSR_f, #-16777216	; 0xff000000
		{0xe368f4ff, "spsr", 0xff000000}, // msr	SPSR_f, #-16777216	; 0xff000000
		{0xe328f8ff, "cpsr", 0xff000000}, // msr	CPSR_f, #16711680	; 0xff0000
		{0xe368f8ff, "spsr", 0xff000000}, // msr	SPSR_f, #16711680	; 0xff0000
		{0xe328fcff, "cpsr", 0xff000000}, // msr	CPSR_f, #65280	; 0xff00
		{0xe368fcff, "spsr", 0xff000000}, // msr	SPSR_f, #65280	; 0xff00
		{0xe328f0ff, "cpsr", 0xff000000}, // msr	CPSR_f, #255	; 0xff
		{0xe368f0ff, "spsr", 0xff000000}, // msr	SPSR_f, #255	; 0xff
		{0xe322f4ff, "cpsr", 0x0000ff00}, // msr	CPSR_x, #-16777216	; 0xff000000
		{0xe362f4ff, "spsr", 0x0000ff00}, // msr	SPSR_x, #-16777216	; 0xff000000
		{0xe322f8ff, "cpsr", 0x0000ff00}, // msr	CPSR_x, #16711680	; 0xff0000
		{0xe362f8ff, "spsr", 0x0000ff00}, // msr	SPSR_x, #16711680	; 0xff0000
		{0xe322fcff, "cpsr", 0x0000ff00}, // msr	CPSR_x, #65280	; 0xff00
		{0xe362fcff, "spsr", 0x0000ff00}, // msr	SPSR_x, #65280	; 0xff00
		{0xe322f0ff, "cpsr", 0x0000ff00}, // msr	CPSR_x, #255	; 0xff
		{0xe362f0ff, "spsr", 0x0000ff00}, // msr	SPSR_x, #255	; 0xff
		{0xe321f4ff, "cpsr", 0x000000ff}, // msr	CPSR_c, #-16777216	; 0xff000000
		{0xe361f4ff, "spsr", 0x000000ff}, // msr	SPSR_c, #-16777216	; 0xff000000
		{0xe321f8ff, "cpsr", 0x000000ff}, // msr	CPSR_c, #16711680	; 0xff0000
		{0xe361f8ff, "spsr", 0x000000ff}, // msr	SPSR_c, #16711680	; 0xff0000
		{0xe321fcff, "cpsr", 0x000000ff}, // msr	CPSR_c, #65280	; 0xff00
		{0xe361fcff, "spsr", 0x000000ff}, // msr	SPSR_c, #65280	; 0xff00
		{0xe321f0ff, "cpsr", 0x000000ff}, // msr	CPSR_c, #255	; 0xff
		{0xe361f0ff, "spsr", 0x000000ff}, // msr	SPSR_c, #255	; 0xff
		{0xe324f4ff, "cpsr", 0x00ff0000}, // msr	CPSR_s, #-16777216	; 0xff000000
		{0xe364f4ff, "spsr", 0x00ff0000}, // msr	SPSR_s, #-16777216	; 0xff000000
		{0xe324f8ff, "cpsr", 0x00ff0000}, // msr	CPSR_s, #16711680	; 0xff0000
		{0xe364f8ff, "spsr", 0x00ff0000}, // msr	SPSR_s, #16711680	; 0xff0000
		{0xe324fcff, "cpsr", 0x00ff0000}, // msr	CPSR_s, #65280	; 0xff00
		{0xe364fcff, "spsr", 0x00ff0000}, // msr	SPSR_s, #65280	; 0xff00
		{0xe324f0ff, "cpsr", 0x00ff0000}, // msr	CPSR_s, #255	; 0xff
		{0xe364f0ff, "spsr", 0x00ff0000}, // msr	SPSR_s, #255	; 0xff
		{0xe326f4ff, "cpsr", 0x00ffff00}, // msr	CPSR_sx, #-16777216	; 0xff000000
		{0xe366f4ff, "spsr", 0x00ffff00}, // msr	SPSR_sx, #-16777216	; 0xff000000
		{0xe326f8ff, "cpsr", 0x00ffff00}, // msr	CPSR_sx, #16711680	; 0xff0000
		{0xe366f8ff, "spsr", 0x00ffff00}, // msr	SPSR_sx, #16711680	; 0xff0000
		{0xe326fcff, "cpsr", 0x00ffff00}, // msr	CPSR_sx, #65280	; 0xff00
		{0xe366fcff, "spsr", 0x00ffff00}, // msr	SPSR_sx, #65280	; 0xff00
		{0xe326f0ff, "cpsr", 0x00ffff00}, // msr	CPSR_sx, #255	; 0xff
		{0xe366f0ff, "spsr", 0x00ffff00}  // msr	SPSR_sx, #255	; 0xff
	};
	for (auto &testCase : testSet)
	{
		Machine::Arm::regval imm = (testCase.instruction & 0xff)
			<< (32 - (((testCase.instruction & 0xf00) >> 8) * 2));
		auto flagInit = 0xd7d7c5c5;
		auto op = decode(testCase.instruction);
		ArmProgramFixture fixture;
		auto &flags = testCaseFlags(fixture.machine, testCase.psr);

		flags.store(flagInit);
		op->execute(fixture.machine);

		std::stringstream ss;
		ss << std::hex << testCase.instruction << ", " << testCase.psr << ", " << testCase.mask
			<< " -> " << flags.dump() << " == "
			<< ((flagInit & ~testCase.mask) | (imm & testCase.mask));
		BOOST_CHECK_MESSAGE(flags.dump() == ((flagInit & ~testCase.mask) | (imm & testCase.mask)),
			ss.str());
	}
}
