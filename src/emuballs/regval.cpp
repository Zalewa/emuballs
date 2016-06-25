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
#include "regval.hpp"

using namespace Emuballs;

DClass<RegVal>
{
public:
	uint32_t val;
};

DPointered(RegVal);

RegVal::RegVal()
{
	d->val = 0;
}

RegVal::RegVal(uint32_t val)
{
	d->val = val;
}

RegVal::~RegVal() {}

RegVal::operator uint32_t() const
{
	return d->val;
}
