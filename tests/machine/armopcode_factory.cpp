#define BOOST_TEST_MODULE machine_armopcode_factory
#include <boost/test/unit_test.hpp>
#include <map>
#include <functional>
#include <list>
#include <cstdint>
#include "src/machine/armopcode_impl.hpp"

struct Fixture
{
	enum class Factory
	{
		opcodeDataProcessingPsrTransfer,
		opcodeMultiply,
		opcodeMultiplyLong,
		opcodeSingleDataSwap,
		opcodeBranchAndExchange,
		opcodeHalfwordDataTransferRegisterOffset,
		opcodeHalfwordDataTransferImmediateOffset,
		opcodeSingleDataTransfer,
		opcodeBlockDataTransfer,
		opcodeBranch,
		opcodeCoprocessorDataTransfer,
		opcodeCoprocessorDataOperation,
		opcodeCoprocessorRegisterTransfer,
		opcodeSoftwareInterrupt
	};

	struct Code
	{
		uint32_t code;
	};

	struct CodeSet
	{
		Machine::OpFactory factory;
		std::list<Code> codes;
	};

	std::map<Factory, CodeSet > codes;

	Fixture()
	{
		codes.emplace(Factory::opcodeMultiply,
			CodeSet {
				Machine::opcodeMultiply, std::list<Code> {
					{0xe0000291}, // mul r0, r1, r2
					{0xe0010192}, // mul r1, r2, r1
					{0xe0100291}, // muls r0, r1, r2
					{0xe0110192}, // muls r1, r2, r1
					{0xe0203291}, // mla r0, r1, r2, r3
					{0xe03bc99a}, // mlas fp, sl, r9, ip
					{0xe0698798}, // mls r9, r8, r7, r8
						}});
		codes.emplace(Factory::opcodeMultiplyLong,
			CodeSet {
				Machine::opcodeMultiplyLong, std::list<Code> {
					{0xe0810594}, // umull r0, r1, r4, r5
					{0xe0932796}, // umulls r2, r3, r6, r7
					{0xe0a10594}, // umlal r0, r1, r4, r5
					{0xe0b32796}, // umlals r2, r3, r6, r7
					{0xe0c98b9a}, // smull r8, r9, sl, fp
					{0xe0d54b9a}, // smulls r4, r5, sl, fp
					{0xe0e98c9a}, // smlal r8, r9, sl, ip
					{0xe0f54998}, // mlals r4, r5, r8, r9
						}});
		codes.emplace(Factory::opcodeBranch,
			CodeSet {
				Machine::opcodeBranch, std::list<Code> {
					{0xeafffffe}, // b 8000
					{0xebfffffd}, // bl 8000
						}});
		codes.emplace(Factory::opcodeBranchAndExchange,
			CodeSet {
				Machine::opcodeBranchAndExchange, std::list<Code> {
					{0xe12fff10}, // bx r0
					{0xe12fff13}, // bx r3
					{0xe12fff1e}, // bx lr
						}});
		codes.emplace(Factory::opcodeSingleDataTransfer,
			CodeSet {
				Machine::opcodeSingleDataTransfer, std::list<Code> {
					{0xe5910000}, // ldr r0, [r1]
					{0xe5910004}, // ldr r0, [r1, #4]
					{0xe5110004}, // ldr r0, [r1, #-4]
					{0xe5b10004}, // ldr r0, [r1, #4]!
					{0xe5310004}, // ldr r0, [r1, #-4]!
					{0xe4910004}, // ldr r0, [r1], #4
					{0xe4110004}, // ldr r0, [r1], #-4
					{0xe7910002}, // ldr r0, [r1, r2]
					{0xe7110002}, // ldr r0, [r1, -r2]
					{0xe7910282}, // ldr r0, [r1, r2, lsl #5]
					{0xe7110842}, // ldr r0, [r1, -r2, asr #16]
					{0xe7b10002}, // ldr r0, [r1, r2]!
					{0xe7310002}, // ldr r0, [r1, -r2]!
					{0xe7b10282}, // ldr r0, [r1, r2, lsl #5]!
					{0xe7310842}, // ldr r0, [r1, -r2, asr #16]!
					{0xe6910002}, // ldr r0, [r1], r2
					{0xe6110002}, // ldr r0, [r1], -r2
					{0xe6910282}, // ldr r0, [r1], r2, lsl #5
					{0xe6110842}, // ldr r0, [r1], -r2, asr #16
					{0xe5910000}, // ldr r0, [r1]
					{0xe5910004}, // ldr r0, [r1, #4]
					{0xe5110004}, // ldr r0, [r1, #-4]
					{0xe5b10004}, // ldr r0, [r1, #4]!
					{0xe5310004}, // ldr r0, [r1, #-4]!
					{0xe4910004}, // ldr r0, [r1], #4
					{0xe4110004}, // ldr r0, [r1], #-4
					{0xe7910002}, // ldr r0, [r1, r2]
					{0xe7110002}, // ldr r0, [r1, -r2]
					{0xe7910282}, // ldr r0, [r1, r2, lsl #5]
					{0xe7110842}, // ldr r0, [r1, -r2, asr #16]
					{0xe7b10002}, // ldr r0, [r1, r2]!
					{0xe7310002}, // ldr r0, [r1, -r2]!
					{0xe7b10282}, // ldr r0, [r1, r2, lsl #5]!
					{0xe7310842}, // ldr r0, [r1, -r2, asr #16]!
					{0xe6910002}, // ldr r0, [r1], r2
					{0xe6110002}, // ldr r0, [r1], -r2
					{0xe6910282}, // ldr r0, [r1], r2, lsl #5
					{0xe6110842}, // ldr r0, [r1], -r2, asr #16
					{0xe5d10000}, // ldrb r0, [r1]
					{0xe5d10004}, // ldrb r0, [r1, #4]
					{0xe5510004}, // ldrb r0, [r1, #-4]
					{0xe5f10004}, // ldrb r0, [r1, #4]!
					{0xe5710004}, // ldrb r0, [r1, #-4]!
					{0xe4d10004}, // ldrb r0, [r1], #4
					{0xe4510004}, // ldrb r0, [r1], #-4
					{0xe7d10002}, // ldrb r0, [r1, r2]
					{0xe7510002}, // ldrb r0, [r1, -r2]
					{0xe7d10282}, // ldrb r0, [r1, r2, lsl #5]
					{0xe7510842}, // ldrb r0, [r1, -r2, asr #16]
					{0xe7f10002}, // ldrb r0, [r1, r2]!
					{0xe7710002}, // ldrb r0, [r1, -r2]!
					{0xe7f10282}, // ldrb r0, [r1, r2, lsl #5]!
					{0xe7710842}, // ldrb r0, [r1, -r2, asr #16]!
					{0xe6d10002}, // ldrb r0, [r1], r2
					{0xe6510002}, // ldrb r0, [r1], -r2
					{0xe6d10282}, // ldrb r0, [r1], r2, lsl #5
					{0xe6510842}, // ldrb r0, [r1], -r2, asr #16
					{0xe1c200d0}, // ldrd r0, [r2]
					{0xe1c200d4}, // ldrd r0, [r2, #4]
					{0xe1e200d4}, // ldrd r0, [r2, #4]!
					{0xe0c200d4}, // ldrd r0, [r2], #4
					{0xe18140d2}, // ldrd r4, [r1, r2]
					{0xe10140d2}, // ldrd r4, [r1, -r2]
					{0xe1a140d2}, // ldrd r4, [r1, r2]!
					{0xe12140d2}, // ldrd r4, [r1, -r2]!
					{0xe08140d2}, // ldrd r4, [r1], r2
					{0xe00140d2}, // ldrd r4, [r1], -r2
					{0xe5810000}, // str r0, [r1]
					{0xe5810004}, // str r0, [r1, #4]
					{0xe5010004}, // str r0, [r1, #-4]
					{0xe5a10004}, // str r0, [r1, #4]!
					{0xe5210004}, // str r0, [r1, #-4]!
					{0xe4810004}, // str r0, [r1], #4
					{0xe4010004}, // str r0, [r1], #-4
					{0xe7810002}, // str r0, [r1, r2]
					{0xe7010002}, // str r0, [r1, -r2]
					{0xe7810282}, // str r0, [r1, r2, lsl #5]
					{0xe7010842}, // str r0, [r1, -r2, asr #16]
					{0xe7a10002}, // str r0, [r1, r2]!
					{0xe7210002}, // str r0, [r1, -r2]!
					{0xe7a10282}, // str r0, [r1, r2, lsl #5]!
					{0xe7210842}, // str r0, [r1, -r2, asr #16]!
					{0xe6810002}, // str r0, [r1], r2
					{0xe6010002}, // str r0, [r1], -r2
					{0xe6810282}, // str r0, [r1], r2, lsl #5
					{0xe6010842}, // str r0, [r1], -r2, asr #16
					{0xe5810000}, // str r0, [r1]
					{0xe5810004}, // str r0, [r1, #4]
					{0xe5010004}, // str r0, [r1, #-4]
					{0xe5a10004}, // str r0, [r1, #4]!
					{0xe5210004}, // str r0, [r1, #-4]!
					{0xe4810004}, // str r0, [r1], #4
					{0xe4010004}, // str r0, [r1], #-4
					{0xe7810002}, // str r0, [r1, r2]
					{0xe7010002}, // str r0, [r1, -r2]
					{0xe7810282}, // str r0, [r1, r2, lsl #5]
					{0xe7010842}, // str r0, [r1, -r2, asr #16]
					{0xe7a10002}, // str r0, [r1, r2]!
					{0xe7210002}, // str r0, [r1, -r2]!
					{0xe7a10282}, // str r0, [r1, r2, lsl #5]!
					{0xe7210842}, // str r0, [r1, -r2, asr #16]!
					{0xe6810002}, // str r0, [r1], r2
					{0xe6010002}, // str r0, [r1], -r2
					{0xe6810282}, // str r0, [r1], r2, lsl #5
					{0xe6010842}, // str r0, [r1], -r2, asr #16
					{0xe5c10000}, // strb r0, [r1]
					{0xe5c10004}, // strb r0, [r1, #4]
					{0xe5410004}, // strb r0, [r1, #-4]
					{0xe5e10004}, // strb r0, [r1, #4]!
					{0xe5610004}, // strb r0, [r1, #-4]!
					{0xe4c10004}, // strb r0, [r1], #4
					{0xe4410004}, // strb r0, [r1], #-4
					{0xe7c10002}, // strb r0, [r1, r2]
					{0xe7410002}, // strb r0, [r1, -r2]
					{0xe7c10282}, // strb r0, [r1, r2, lsl #5]
					{0xe7410842}, // strb r0, [r1, -r2, asr #16]
					{0xe7e10002}, // strb r0, [r1, r2]!
					{0xe7610002}, // strb r0, [r1, -r2]!
					{0xe7e10282}, // strb r0, [r1, r2, lsl #5]!
					{0xe7610842}, // strb r0, [r1, -r2, asr #16]!
					{0xe6c10002}, // strb r0, [r1], r2
					{0xe6410002}, // strb r0, [r1], -r2
					{0xe6c10282}, // strb r0, [r1], r2, lsl #5
					{0xe6410842}, // strb r0, [r1], -r2, asr #16
					{0xe1c200f0}, // strd r0, [r2]
					{0xe1c200f4}, // strd r0, [r2, #4]
					{0xe1e200f4}, // strd r0, [r2, #4]!
					{0xe0c200f4}, // strd r0, [r2], #4
					{0xe18140f2}, // strd r4, [r1, r2]
					{0xe10140f2}, // strd r4, [r1, -r2]
					{0xe1a140f2}, // strd r4, [r1, r2]!
					{0xe12140f2}, // strd r4, [r1, -r2]!
					{0xe08140f2}, // strd r4, [r1], r2
					{0xe00140f2}, // strd r4, [r1], -r2
						}});
		codes.emplace(Factory::opcodeBlockDataTransfer,
			CodeSet {
				Machine::opcodeBlockDataTransfer, std::list<Code> {
					{0xe89201c0}, // ldm r2, {r6, r7, r8}
					{0xe99201c0}, // ldmib r2, {r6, r7, r8}
					{0xe812ffff}, // ldmda r2, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, pc}
					{0xe91201c0}, // ldmdb r2, {r6, r7, r8}
					{0xe8b201c0}, // ldm r2!, {r6, r7, r8}
					{0xe9b201c0}, // ldmib r2!, {r6, r7, r8}
					{0xe832fffb}, // ldmda r2!, {r0, r1, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, pc}
					{0xe93201c0}, // ldmdb r2!, {r6, r7, r8}
					{0xe8d201c0}, // ldm r2, {r6, r7, r8}^
					{0xe9d201c0}, // ldmib r2, {r6, r7, r8}^
					{0xe852ffff}, // ldmda r2, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, pc}^
					{0xe95201c0}, // ldmdb r2, {r6, r7, r8}^
					{0xe8f28000}, // ldm r2!, {pc}^
					{0xe9f28000}, // ldmib r2!, {pc}^
					{0xe872fffb}, // ldmda r2!, {r0, r1, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, pc}^
					{0xe9728000}, // ldmdb r2!, {pc}^
					{0xe88201c0}, // stm r2, {r6, r7, r8}
					{0xe98201c0}, // stmib r2, {r6, r7, r8}
					{0xe802ffff}, // stmda r2, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, pc}
					{0xe90201c0}, // stmdb r2, {r6, r7, r8}
					{0xe8a201c0}, // stmia r2!, {r6, r7, r8}
					{0xe9a201c0}, // stmib r2!, {r6, r7, r8}
					{0xe822fffb}, // stmda r2!, {r0, r1, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, pc}
					{0xe92201c0}, // stmdb r2!, {r6, r7, r8}
					{0xe8c201c0}, // stmia r2, {r6, r7, r8}^
					{0xe9c201c0}, // stmib r2, {r6, r7, r8}^
					{0xe842ffff}, // stmda r2, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, pc}^
					{0xe94201c0}, // stmdb r2, {r6, r7, r8}^
						}});
		codes.emplace(Factory::opcodeSingleDataSwap,
			CodeSet {
				Machine::opcodeSingleDataSwap, std::list<Code> {
					{0xe1010090}, // swp r0, r0, [r1]
					{0xe1010092}, // swp r0, r2, [r1]
					{0xe1410090}, // swpb r0, r0, [r1]
					{0xe1410092}, // swpb r0, r2, [r1]
						}});
		codes.emplace(Factory::opcodeSoftwareInterrupt,
			CodeSet {
				Machine::opcodeSoftwareInterrupt, std::list<Code> {
					{0xef000001}, // svc 0x00000001
					{0xefffffff}, // svc 0x00ffffff
					{0xef00ffff}, // svc 0x0000ffff
						}});
		codes.emplace(Factory::opcodeDataProcessingPsrTransfer,
			CodeSet {
				Machine::opcodeDataProcessingPsrTransfer, std::list<Code> {
					{0xe3100ffe}, // tst r0, #1016 ; 0x3f8
					{0xe1100001}, // tst r0, r1
					{0xe1110155}, // tst r1, r5, asr r1
					{0xe13a0009}, // teq sl, r9
					{0xe33a0ffe}, // teq sl, #1016 ; 0x3f8
					{0xe13a0159}, // teq sl, r9, asr r1
					{0xe1520009}, // cmp r2, r9
					{0xe3520c19}, // cmp r2, #6400 ; 0x1900
					{0xe1520159}, // cmp r2, r9, asr r1
					{0xe1520149}, // cmp r2, r9, asr #2
					{0xe1700007}, // cmn r0, r7
					{0xe3700e1e}, // cmn r0, #480 ; 0x1e0
					{0xe1700417}, // cmn r0, r7, lsl r4
					{0xe0810002}, // add r0, r1, r2
					{0xe0910002}, // adds r0, r1, r2
					{0xe0800511}, // add r0, r0, r1, lsl r5
					{0xe0900781}, // adds r0, r0, r1, lsl #15
					{0xe0a10002}, // adc r0, r1, r2
					{0xe0b10002}, // adcs r0, r1, r2
					{0xe0410002}, // sub r0, r1, r2
					{0xe0510002}, // subs r0, r1, r2
					{0xe0c10002}, // sbc r0, r1, r2
					{0xe0d10002}, // sbcs r0, r1, r2
					{0xe0610002}, // rsb r0, r1, r2
					{0xe0710002}, // rsbs r0, r1, r2
					{0xe0600151}, // rsb r0, r0, r1, asr r1
					{0xe0e10002}, // rsc r0, r1, r2
					{0xe0f10002}, // rscs r0, r1, r2
					{0xe0010002}, // and r0, r1, r2
					{0xe0100151}, // ands r0, r0, r1, asr r1
					{0xe0110002}, // ands r0, r1, r2
					{0xe0210002}, // eor r0, r1, r2
					{0xe0310002}, // eors r0, r1, r2
					{0xe1810002}, // orr r0, r1, r2
					{0xe1910002}, // orrs r0, r1, r2
					{0xe1900044}, // orrs r0, r0, r4, asr #32
					{0xe1900114}, // orrs r0, r0, r4, lsl r1
					{0xe1c10002}, // bic r0, r1, r2
					{0xe1d10002}, // bics r0, r1, r2
					{0xe1a00001}, // mov r0, r1
					{0xe3a00000}, // mov r0, #0
					{0xe3a000ff}, // mov r0, #255 ; 0xff
					{0xe3a00cff}, // mov r0, #65280 ; 0xff00
					{0xe1a00511}, // lsl r0, r1, r5
					{0xe1a00081}, // lsl r0, r1, #1
					{0xe1b00001}, // movs r0, r1
					{0xe3b00000}, // movs r0, #0
					{0xe3b000ff}, // movs r0, #255 ; 0xff
					{0xe3b00cff}, // movs r0, #65280 ; 0xff00
					{0xe1e00001}, // mvn r0, r1
					{0xe3e00000}, // mvn r0, #0
					{0xe3e000ff}, // mvn r0, #255 ; 0xff
					{0xe3e00cff}, // mvn r0, #65280 ; 0xff00
					{0xe1e00451}, // mvn r0, r1, asr r4
					{0xe1f00001}, // mvns r0, r1
					{0xe3f00000}, // mvns r0, #0
					{0xe3f000ff}, // mvns r0, #255 ; 0xff
					{0xe3f00cff}, // mvns r0, #65280 ; 0xff00
					{0xe10f0000}, // mrs r0, CPSR
					{0xe14f0000}, // mrs r0, SPSR
					{0xe169f000}, // msr SPSR_fc, r0
					{0xe168f001}, // msr SPSR_f, r1
					{0xe161f005}, // msr SPSR_c, r5
					{0xe361f20f}, // msr SPSR_c, #-268435456 ; 0xf0000000
					{0xe129f000}, // msr CPSR_fc, r0
					{0xe321f20f}, // msr CPSR_c, #-268435456 ; 0xf0000000
					{0xe128f004}, // msr CPSR_f, r4
					{0xe328f20f}, // msr CPSR_f, #-268435456 ; 0xf0000000
						}});
		codes.emplace(Factory::opcodeHalfwordDataTransferRegisterOffset,
			CodeSet {
				Machine::opcodeHalfwordDataTransferRegisterOffset, std::list<Code> {
					{0xe1d100d0}, // ldrsb r0, [r1]
					{0xe1d100d4}, // ldrsb r0, [r1, #4]
					{0xe15100d4}, // ldrsb r0, [r1, #-4]
					{0xe1f100d4}, // ldrsb r0, [r1, #4]!
					{0xe17100d4}, // ldrsb r0, [r1, #-4]!
					{0xe0d100d4}, // ldrsb r0, [r1], #4
					{0xe05100d4}, // ldrsb r0, [r1], #-4
					{0xe19100d2}, // ldrsb r0, [r1, r2]
					{0xe11100d2}, // ldrsb r0, [r1, -r2]
					{0xe1b100d2}, // ldrsb r0, [r1, r2]!
					{0xe13100d2}, // ldrsb r0, [r1, -r2]!
					{0xe09100d2}, // ldrsb r0, [r1], r2
					{0xe01100d2}, // ldrsb r0, [r1], -r2
					{0xe1d100b0}, // ldrh r0, [r1]
					{0xe1d100b4}, // ldrh r0, [r1, #4]
					{0xe15100b4}, // ldrh r0, [r1, #-4]
					{0xe1f100b4}, // ldrh r0, [r1, #4]!
					{0xe17100b4}, // ldrh r0, [r1, #-4]!
					{0xe0d100b4}, // ldrh r0, [r1], #4
					{0xe05100b4}, // ldrh r0, [r1], #-4
					{0xe19100b2}, // ldrh r0, [r1, r2]
					{0xe11100b2}, // ldrh r0, [r1, -r2]
					{0xe1b100b2}, // ldrh r0, [r1, r2]!
					{0xe13100b2}, // ldrh r0, [r1, -r2]!
					{0xe09100b2}, // ldrh r0, [r1], r2
					{0xe01100b2}, // ldrh r0, [r1], -r2
					{0xe1d100f0}, // ldrsh r0, [r1]
					{0xe1d100f4}, // ldrsh r0, [r1, #4]
					{0xe15100f4}, // ldrsh r0, [r1, #-4]
					{0xe1f100f4}, // ldrsh r0, [r1, #4]!
					{0xe17100f4}, // ldrsh r0, [r1, #-4]!
					{0xe0d100f4}, // ldrsh r0, [r1], #4
					{0xe05100f4}, // ldrsh r0, [r1], #-4
					{0xe19100f2}, // ldrsh r0, [r1, r2]
					{0xe11100f2}, // ldrsh r0, [r1, -r2]
					{0xe1b100f2}, // ldrsh r0, [r1, r2]!
					{0xe13100f2}, // ldrsh r0, [r1, -r2]!
					{0xe09100f2}, // ldrsh r0, [r1], r2
					{0xe01100f2}, // ldrsh r0, [r1], -r2
					{0xe1c100b0}, // strh r0, [r1]
					{0xe1c100b4}, // strh r0, [r1, #4]
					{0xe14100b4}, // strh r0, [r1, #-4]
					{0xe1e100b4}, // strh r0, [r1, #4]!
					{0xe16100b4}, // strh r0, [r1, #-4]!
					{0xe0c100b4}, // strh r0, [r1], #4
					{0xe04100b4}, // strh r0, [r1], #-4
					{0xe18100b2}, // strh r0, [r1, r2]
					{0xe10100b2}, // strh r0, [r1, -r2]
					{0xe1a100b2}, // strh r0, [r1, r2]!
					{0xe12100b2}, // strh r0, [r1, -r2]!
					{0xe08100b2}, // strh r0, [r1], r2
					{0xe00100b2}, // strh r0, [r1], -r2
						}});
		codes.emplace(Factory::opcodeHalfwordDataTransferImmediateOffset,
			CodeSet {
				Machine::opcodeHalfwordDataTransferImmediateOffset, std::list<Code> {
						}});
		codes.emplace(Factory::opcodeCoprocessorDataTransfer,
			CodeSet {
				Machine::opcodeCoprocessorDataTransfer, std::list<Code> {
					{0xeef210e3}, // cdp 0, 15, cr1, cr2, cr3, {7}
					{0xeee2ff83}, // cdp 15, 14, cr15, cr2, cr3, {4}
					{0xfef210e3}, // cdp2 0, 15, cr1, cr2, cr3, {7}
					{0xfee2ff83}, // cdp2 15, 14, cr15, cr2, cr3, {4}
						}});
		codes.emplace(Factory::opcodeCoprocessorDataOperation,
			CodeSet {
				Machine::opcodeCoprocessorDataOperation, std::list<Code> {
					{0xed932600}, // ldc 6, cr2, [r3]
					{0xed91f513}, // cfldr32 mvfx15, [r1, #76] ; 0x4c
					{0xed11f513}, // cfldr32 mvfx15, [r1, #-76] ; 0xffffffb4
					{0xedb1c4e6}, // cfldrs mvf12, [r1, #920]! ; 0x398
					{0xed31c4e6}, // cfldrs mvf12, [r1, #-920]! ; 0xfffffc68
					{0xecb1b3af}, // ldc 3, cr11, [r1], #700 ; 0x2bc
					{0xec31b3af}, // ldc 3, cr11, [r1], #-700 ; 0xfffffd44
					{0xedd32600}, // ldcl 6, cr2, [r3]
					{0xedd1f513}, // cfldr64 mvdx15, [r1, #76] ; 0x4c
					{0xed51f513}, // cfldr64 mvdx15, [r1, #-76] ; 0xffffffb4
					{0xedf1c4e6}, // cfldrd mvd12, [r1, #920]! ; 0x398
					{0xed71c4e6}, // cfldrd mvd12, [r1, #-920]! ; 0xfffffc68
					{0xecf1b3af}, // ldcl 3, cr11, [r1], #700 ; 0x2bc
					{0xec71b3af}, // ldcl 3, cr11, [r1], #-700 ; 0xfffffd44
					{0xfd932600}, // ldc2 6, cr2, [r3]
					{0xfd91f513}, // ldc2 5, cr15, [r1, #76] ; 0x4c
					{0xfd11f513}, // ldc2 5, cr15, [r1, #-76] ; 0xffffffb4
					{0xfdb1c4e6}, // ldc2 4, cr12, [r1, #920]! ; 0x398
					{0xfd31c4e6}, // ldc2 4, cr12, [r1, #-920]! ; 0xfffffc68
					{0xfcb1b3af}, // ldc2 3, cr11, [r1], #700 ; 0x2bc
					{0xfc31b3af}, // ldc2 3, cr11, [r1], #-700 ; 0xfffffd44
					{0xfdd32600}, // ldc2l 6, cr2, [r3]
					{0xfdd1f513}, // ldc2l 5, cr15, [r1, #76] ; 0x4c
					{0xfd51f513}, // ldc2l 5, cr15, [r1, #-76] ; 0xffffffb4
					{0xfdf1c4e6}, // ldc2l 4, cr12, [r1, #920]! ; 0x398
					{0xfd71c4e6}, // ldc2l 4, cr12, [r1, #-920]! ; 0xfffffc68
					{0xfcf1b3af}, // ldc2l 3, cr11, [r1], #700 ; 0x2bc
					{0xfc71b3af}, // ldc2l 3, cr11, [r1], #-700 ; 0xfffffd44
					{0xed832600}, // stc 6, cr2, [r3]
					{0xed81f513}, // cfstr32 mvfx15, [r1, #76] ; 0x4c
					{0xed01f513}, // cfstr32 mvfx15, [r1, #-76] ; 0xffffffb4
					{0xeda1c4e6}, // cfstrs mvf12, [r1, #920]! ; 0x398
					{0xed21c4e6}, // cfstrs mvf12, [r1, #-920]! ; 0xfffffc68
					{0xeca1b3af}, // stc 3, cr11, [r1], #700 ; 0x2bc
					{0xec21b3af}, // stc 3, cr11, [r1], #-700 ; 0xfffffd44
					{0xedc32600}, // stcl 6, cr2, [r3]
					{0xedc1f513}, // cfstr64 mvdx15, [r1, #76] ; 0x4c
					{0xed41f513}, // cfstr64 mvdx15, [r1, #-76] ; 0xffffffb4
					{0xede1c4e6}, // cfstrd mvd12, [r1, #920]! ; 0x398
					{0xed61c4e6}, // cfstrd mvd12, [r1, #-920]! ; 0xfffffc68
					{0xece1b3af}, // stcl 3, cr11, [r1], #700 ; 0x2bc
					{0xec61b3af}, // stcl 3, cr11, [r1], #-700 ; 0xfffffd44
					{0xfd832600}, // stc2 6, cr2, [r3]
					{0xfd81f513}, // stc2 5, cr15, [r1, #76] ; 0x4c
					{0xfd01f513}, // stc2 5, cr15, [r1, #-76] ; 0xffffffb4
					{0xfda1c4e6}, // stc2 4, cr12, [r1, #920]! ; 0x398
					{0xfd21c4e6}, // stc2 4, cr12, [r1, #-920]! ; 0xfffffc68
					{0xfca1b3af}, // stc2 3, cr11, [r1], #700 ; 0x2bc
					{0xfc21b3af}, // stc2 3, cr11, [r1], #-700 ; 0xfffffd44
					{0xfdc32600}, // stc2l 6, cr2, [r3]
					{0xfdc1f513}, // stc2l 5, cr15, [r1, #76] ; 0x4c
					{0xfd41f513}, // stc2l 5, cr15, [r1, #-76] ; 0xffffffb4
					{0xfde1c4e6}, // stc2l 4, cr12, [r1, #920]! ; 0x398
					{0xfd61c4e6}, // stc2l 4, cr12, [r1, #-920]! ; 0xfffffc68
					{0xfce1b3af}, // stc2l 3, cr11, [r1], #700 ; 0x2bc
					{0xfc61b3af}, // stc2l 3, cr11, [r1], #-700 ; 0xfffffd44
						}});
		codes.emplace(Factory::opcodeCoprocessorRegisterTransfer,
			CodeSet {
				Machine::opcodeCoprocessorRegisterTransfer, std::list<Code> {
					{0xeee42715}, // mcr 7, 7, r2, cr4, cr5, {0}
					{0xeee957b2}, // mcr 7, 7, r5, cr9, cr2, {5}
					{0xfee40415}, // mcr2 4, 7, r0, cr4, cr5, {0}
					{0xfe6877f1}, // mcr2 7, 3, r7, cr8, cr1, {7}
					{0xec4429ea}, // mcrr 9, 14, r2, r4, cr10
					{0xec4327b5}, // mcrr 7, 11, r2, r3, cr5
					{0xfc4429ea}, // mcrr2 9, 14, r2, r4, cr10
					{0xfc4327b5}, // mcrr2 7, 11, r2, r3, cr5
					{0xeef42715}, // mrc 7, 7, r2, cr4, cr5, {0}
					{0xeef957b2}, // mrc 7, 7, r5, cr9, cr2, {5}
					{0xfef40415}, // mrc2 4, 7, r0, cr4, cr5, {0}
					{0xfe7877f1}, // mrc2 7, 3, r7, cr8, cr1, {7}
					{0xec5429ea}, // mrrc 9, 14, r2, r4, cr10
					{0xec5327b5}, // mrrc 7, 11, r2, r3, cr5
					{0xfc5429ea}, // mrrc2 9, 14, r2, r4, cr10
					{0xfc5327b5}, // mrrc2 7, 11, r2, r3, cr5
						}});

		fillInConditionals();
	}

	void fillInConditionals()
	{
		for (auto &codebatch : codes)
		{
			auto &staticCodes = codebatch.second.codes;
			decltype(codebatch.second.codes) conditionCodes;
			for (auto const & code : codebatch.second.codes)
			{
				if ((code.code & 0xf0000000) == 0xf0000000)
				{
					continue; // skip
				}
				for (auto i = 0; i < 0xf; ++i)
				{
					auto literal = code.code;
					literal &= 0x0fffffff;
					literal |= i << 28;
					conditionCodes.push_back({literal});
				}
			}
			staticCodes = conditionCodes;
		}
	}
};

std::ostream& operator<< (std::ostream & os, Fixture::Factory factory)
{
	switch (factory)
	{
	case Fixture::Factory::opcodeDataProcessingPsrTransfer:
		return os << "opcodeDataProcessingPsrTransfer";
	case Fixture::Factory::opcodeMultiply:
		return os << "opcodeMultiply";
	case Fixture::Factory::opcodeMultiplyLong:
		return os << "opcodeMultiplyLong";
	case Fixture::Factory::opcodeSingleDataSwap:
		return os << "opcodeSingleDataSwap";
	case Fixture::Factory::opcodeBranchAndExchange:
		return os << "opcodeBranchAndExchange";
	case Fixture::Factory::opcodeHalfwordDataTransferRegisterOffset:
		return os << "opcodeHalfwordDataTransferRegisterOffset";
	case Fixture::Factory::opcodeHalfwordDataTransferImmediateOffset:
		return os << "opcodeHalfwordDataTransferImmediateOffset";
	case Fixture::Factory::opcodeSingleDataTransfer:
		return os << "opcodeSingleDataTransfer";
	case Fixture::Factory::opcodeBlockDataTransfer:
		return os << "opcodeBlockDataTransfer";
	case Fixture::Factory::opcodeBranch:
		return os << "opcodeBranch";
	case Fixture::Factory::opcodeCoprocessorDataTransfer:
		return os << "opcodeCoprocessorDataTransfer";
	case Fixture::Factory::opcodeCoprocessorDataOperation:
		return os << "opcodeCoprocessorDataOperation";
	case Fixture::Factory::opcodeCoprocessorRegisterTransfer:
		return os << "opcodeCoprocessorRegisterTransfer";
	case Fixture::Factory::opcodeSoftwareInterrupt:
		return os << "opcodeSoftwareInterrupt";
	default:
		return os << "unknown Fixture::Factory" << static_cast<int>(factory);
	}
}

std::ostream &operator<<(std::ostream &os, const Fixture::Code &code)
{
	auto flags = os.flags();
	auto width = os.width();
	auto fill = os.fill();
	os << "0x";
	os.flags(std::ios::right | std::ios::hex);
	os.fill('0');
	os.width(8);
	os << code.code;
	os.flags(flags);
	os.fill(fill);
	os.width(width);
	return os;
}

BOOST_FIXTURE_TEST_SUITE(armopcode_factory, Fixture)

BOOST_AUTO_TEST_CASE(run)
{
	for (auto const &codebatch : codes)
	{
		auto factory = codebatch.second.factory;
		for (auto const &code : codebatch.second.codes)
		{
			std::stringstream ss;
			ss << codebatch.first << " " << code;
			Machine::ArmOpcodePtr opcode = factory(code.code);
			BOOST_CHECK_MESSAGE(opcode != nullptr, ss.str());
		}
		for (auto const &otherbatch : codes)
		{
			if (codebatch.first == otherbatch.first)
			{
				continue;
			}
			for (auto const &code : otherbatch.second.codes)
			{
				std::stringstream ss;
				ss << codebatch.first << " <- " << otherbatch.first << " " << code;
				Machine::ArmOpcodePtr opcode = factory(code.code);
				BOOST_CHECK_MESSAGE(opcode == nullptr, ss.str());
			}
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
