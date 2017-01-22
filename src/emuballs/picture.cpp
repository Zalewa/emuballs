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
#include "picture.hpp"

using namespace Emuballs;

DClass<Picture>
{
public:
	size_t width;
	size_t height;
};

DPointered(Picture)

Picture::Picture()
{
	d->width = 0;
	d->height = 0;
}

Picture::Picture(size_t width, size_t height)
{
	d->width = width;
	d->height = height;
}

Picture::~Picture()
{
}

size_t Picture::height() const
{
	return d->height;
}

size_t Picture::width() const
{
	return d->width;
}

bool Picture::valid() const
{
	return d->height > 0 && d->width > 0;
}
