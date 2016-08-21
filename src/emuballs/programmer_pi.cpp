#include "programmer_pi.hpp"

#include <iostream>
#include "device.hpp"
#include "memory.hpp"

using namespace Emuballs;

void ProgrammerPi::load(std::istream &in)
{
	static const auto CHUNK_SIZE = 1024 * 1024;
	char chunk[CHUNK_SIZE];
	uint32_t memoryOffset = 0;
	while (in)
	{
		in.read(&chunk[0], sizeof(char) * CHUNK_SIZE);
		auto gotCount = in.gcount();
		for (auto i = 0; i < gotCount; ++i)
		{
			device().memory().putByte(memoryOffset, chunk[i]);
			++memoryOffset;
		}
	}
}
