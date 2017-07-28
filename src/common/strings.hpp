/*
 * Copyright 2016 Zalewa <zalewapl@gmail.com>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <string>
#include <sstream>

namespace Strings
{
int compareCaseInsensitive(std::string a, std::string b);

template<class Range, class Type, class Concatenator>
Type concat(const Range &range, const Type &delim)
{
	Concatenator c;
	for (const Type &element : range)
	{
		c << element;
		c << delim;
	}
	Type str = c.str();
	if (str.size() >= delim.size())
		str.resize(str.size() - delim.size());
	return str;
}

template<class Range>
std::string concat(const Range &range, std::string delim)
{
	return concat<Range, std::string, std::ostringstream>(range, delim);
}

}
