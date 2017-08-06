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

#include "emuballs/export.h"
#include <cstdint>

namespace Emuballs
{

struct EMUBALLS_API Color
{
public:
	uint8_t r, g, b, a;

	Color()
	{
		r = g = b = a = 0;
	}

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
};

enum class BitDepth : int
{
	HighColor = 16
};

}
