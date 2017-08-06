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

#include "emuballs/color.hpp"
#include "emuballs/dptr.hpp"
#include "emuballs/export.h"
#include <vector>

namespace Emuballs
{
class EMUBALLS_API Canvas
{
public:
	Canvas();
	Canvas(const Canvas &other) = delete;
	Canvas &operator=(const Canvas &other) = delete;
	virtual ~Canvas();

	virtual void begin() = 0;
	virtual void end() = 0;

	/**
	 * Draw a whole picture on screen.
	 *
	 * The default implementation is inefficient and will just drawPixel() in loops.
	 *
	 * @param x
	 *     Starting x position from top-left corner.
	 * @param y
	 *     Starting y position from top-left corner.
	 * @param width
	 *     Width of picture in pixels.
	 * @param height
	 *     Height of picture in pixels.
	 * @param bitsPerPixel
	 *     Bit depth of the picture.
	 * @param pitch
	 *     How many bytes are per row of pixels.
	 * @param pixels
	 *     Pixel data.
	 */
	virtual void drawPicture(int32_t x, int32_t y, int32_t width, int32_t height,
		int32_t bitsPerPixel, int32_t pitch, const std::vector<uint8_t> &pixels);
	virtual void drawPixel(int32_t x, int32_t y, const Color &color) = 0;
	virtual void changeSize(int32_t width, int32_t height, BitDepth depth) = 0;

private:
	DPtr<Canvas> d;
};

}
