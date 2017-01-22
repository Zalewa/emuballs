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
#include "armgpu.hpp"

#include "canvas.hpp"
#include "color.hpp"
#include "picture.hpp"

using namespace Emuballs;
using namespace Emuballs::Arm;

DClass<Gpu>
{
public:
	Memory *memory;
};

DPointered(Gpu)

Gpu::Gpu(Memory &memory)
{
	d->memory = &memory;
}

void Gpu::cycle()
{
}

void Gpu::draw(Canvas &canvas)
{
	canvas.begin();
	canvas.setPicture(Picture(640, 480));
	for (int x = 0; x < 640; ++x)
	{
		for (int y = 0; y < 480; ++y)
		{
			canvas.drawPixel(x, y, Color(x % 255, 0, y % 255));
		}
	}
	canvas.drawPixel(0, 0, Color(255, 0, 255));
	canvas.end();
}
