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
#include "emuballs/canvas.hpp"

#include "emuballs/color.hpp"
#include "dptr_impl.hpp"
#include <sstream>

namespace Emuballs
{
DClass<Canvas>
{
public:
	void drawPicture16(Canvas &canvas, int32_t x, int32_t y, int32_t width, int32_t height, const std::vector<uint8_t> &pixels)
	{
		int32_t pitch = 0;
		for (int32_t yPixel = 0; yPixel < height; ++yPixel)
		{
			for (int32_t xPixel = 0; xPixel < width; ++xPixel)
			{
				uint8_t rawcolor[2];
				rawcolor[1] = pixels[pitch];
				rawcolor[0] = pixels[pitch + 1];

				canvas.drawPixel(x + xPixel, y + yPixel, Color(
						(((rawcolor[0] & 0b11111000) >> 3) * 255) / 31,
						((((rawcolor[0] & 0b111) << 3) | ((rawcolor[1] & 0b11100000) >> 5)) * 255) / 63,
						(rawcolor[1] & 0b11111) * 255 / 31
						)
					);

				pitch += 2;
			}
		}

	}
};

DPointered(Canvas);

Canvas::Canvas() {}
Canvas::~Canvas() {}

void Canvas::drawPicture(int32_t x, int32_t y, int32_t width, int32_t height,
	int32_t bitsPerPixel, int32_t pitch, const std::vector<uint8_t> &pixels)
{
	switch (static_cast<BitDepth>(bitsPerPixel))
	{
	case BitDepth::HighColor:
		d->drawPicture16(*this, x, y, width, height, pixels);
		break;
	default:
	{
		std::stringstream ss;
		ss << "unhandled bitsPerPixel " << bitsPerPixel;
		throw std::logic_error(ss.str());
	}
	}
}
}
