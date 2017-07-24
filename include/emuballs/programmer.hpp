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
#include "emuballs/dptr.hpp"

namespace Emuballs
{

class Device;

class EMUBALLS_API Programmer
{
public:
	Programmer(Device &device);
	virtual ~Programmer();

	/**
	 * @throw ProgramLoadError
	 */
	virtual void load(std::istream &) = 0;

protected:
	Device &device();

private:
	DPtr<Programmer> d;
};

}
