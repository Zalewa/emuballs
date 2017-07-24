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

#include "emuballs/dptr.hpp"
#include "emuballs/export.h"

namespace Emuballs
{
struct Color;

class EMUBALLS_API Canvas
{
public:
	Canvas();
	Canvas(const Canvas &other) = delete;
	Canvas &operator=(const Canvas &other) = delete;
	virtual ~Canvas();

	virtual void begin() = 0;
	virtual void end() = 0;

	virtual void drawPixel(int x, int y, const Color &color) = 0;
	virtual void changeSize(int width, int height) = 0;

private:
	DPtr<Canvas> d;
};

}
