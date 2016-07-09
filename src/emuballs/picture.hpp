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

#include "dptr.hpp"
#include "export.h"
#include <cstdint>
#include <vector>

namespace Emuballs
{

/**
 * Raw picture structure.
 */
class MACHINE_API Picture
{
public:
	Picture();
	Picture(size_t width, size_t height, size_t bpp);
	virtual ~Picture();

	size_t bpp() const;
	size_t height() const;
	size_t width() const;

	const std::vector<uint8_t> &data() const;
	std::vector<uint8_t> &data();

	bool valid() const;

private:
	DPtr<Picture> d;
};

}