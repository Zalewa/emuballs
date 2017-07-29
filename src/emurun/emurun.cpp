/*
 * Copyright 2017 Zalewa <zalewapl@gmail.com>.
 *
 * This file is part of Emuballs Emurun.
 *
 * Emuballs Emurun is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Emuballs Emurun is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Emuballs Emurun.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <csignal>
#include <cstdint>
#include <codecvt>
#include <locale>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "emuballs/device.hpp"
#include "emuballs/programmer.hpp"
#include "emuballs/registerset.hpp"
#include "emuballs/regval.hpp"

const std::string VERSION = "0.0-alpha";

bool keepRunning = true;

void term(int param)
{
	keepRunning = false;
}

void listDevices()
{
	std::list<Emuballs::DeviceFactory> devices = Emuballs::listDevices();
	for (const auto &device : devices)
		std::cout << device.name() << std::endl;
}

int execute(const std::string &deviceName, const std::string &programPath,
	int64_t maxCycles = -1)
{
	Emuballs::DevicePtr device = nullptr;

	// Create device.
	for (auto factory : Emuballs::listDevices())
	{
		if (factory.name() == deviceName)
		{
			device = factory.create();
			break;
		}
	}
	if (device == nullptr)
	{
		std::cerr << "wrong device name" << std::endl;
		return 3;
	}

	// Load program.
	std::ifstream program(programPath, std::ios::in | std::ios::binary);
	if (!program.is_open())
	{
		std::cerr << "program file cannot be opened" << std::endl;
		return 4;
	}
	device->programmer().load(program);
	program.close();

	// Execute.
	int64_t cycleIdx = 0;
	while (keepRunning && (maxCycles < 0 || cycleIdx < maxCycles))
	{
		device->cycle();
		++cycleIdx;
	}

	// Summary.
	std::cout << "cycles=" << cycleIdx << std::endl;
	for (Emuballs::NamedRegister &reg : device->registers().registers())
	{
		bool firstName = true;
		std::stringstream ss;
		for (auto name : reg.names())
		{
			if (!firstName)
				ss << ",";
			ss << name;
			firstName = false;
		}
		ss << "=0x" << std::hex << reg.value();
		std::cout << ss.str() << std::endl;
	}
	return 0;
}


int main(int argc, char **argv)
{
	std::string deviceName;
	std::string programPath;
	int64_t maxCycles = -1;

	// Args
	std::cerr << "Emuballs Emurun " << VERSION << std::endl;
	if (argc < 2)
	{
		std::cerr << "Usage: " << std::endl;
		std::cerr << "    " << argv[0] << " \"<Device Name>\" <program_path> [max_cycles]"
			<< std::endl;
		std::cerr << "    " << argv[0] << " -l       -- list devices" << std::endl;
		return 2;
	}
	deviceName = argv[1];
	if (argc >= 3)
		programPath = argv[2];
	if (argc >= 4)
		maxCycles = std::stoll(argv[3]);

	if (deviceName != "-l")
	{
		if (argc < 3)
		{
			std::cerr << "not enough arguments" << std::endl;
			return 2;
		}
		std::cerr << "Dev. name: " << deviceName << std::endl;
		std::cerr << "Program path: " << programPath << std::endl;
		if (maxCycles != -1)
			std::cerr << "Max. cycles: " << maxCycles << std::endl;
	}

	// Signals.
	signal(SIGINT, term);
	signal(SIGTERM, term);

	// Run.
	int ec = 0;
	if (deviceName == "-l")
		listDevices();
	else
		ec = execute(deviceName, programPath, maxCycles);
	return ec;
}
