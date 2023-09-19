/*
 * Copyright 2016 Zalewa <zalewapl@gmail.com>.
 *
 * This file is part of Emuballs.
 *
 * Emuballs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Emuballs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Emuballs.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>

namespace Emuballs
{

namespace Arm
{

class Machine;

class Opcode
{
public:
	Opcode(uint32_t code);
	virtual ~Opcode() = default;

	inline uint32_t code() const
	{
		return m_code;
	}

	void execute(Machine &machine);
	virtual void validate();

protected:
	virtual void run(Machine &machine) = 0;

private:
	uint32_t m_code;
};

typedef std::shared_ptr<Opcode> OpcodePtr;
typedef std::function<OpcodePtr(uint32_t)> OpFactory;

}

}
