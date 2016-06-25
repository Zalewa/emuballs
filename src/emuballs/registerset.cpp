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
#include "registerset.hpp"

#include "errors.hpp"
#include "regval.hpp"
#include "strings.hpp"
#include <algorithm>

using namespace Emuballs;

DClass<NamedRegister>
{
public:
	std::list<std::string> names;
	RegVal val;
};

DPointered(NamedRegister);

NamedRegister::NamedRegister()
{
	d->val = 0;
}

NamedRegister::NamedRegister(const std::string &name, const RegVal &val)
{
	d->names.push_back(name);
	d->val = val;
}

NamedRegister::NamedRegister(const std::list<std::string> &names, const RegVal &val)
{
	d->names = names;
	d->val = val;
}

NamedRegister::~NamedRegister()
{
}

std::list<std::string> NamedRegister::names() const
{
	return d->names;
}

bool NamedRegister::isNameMatch(const std::string &name) const
{
	for (const auto &selfName : d->names)
	{
		if (Strings::compareCaseInsensitive(selfName, name) == 0)
			return true;
	}
	return false;
}

RegVal NamedRegister::value() const
{
	return d->val;
}

///////////////////////////////////////////////////////////////////////////

DClass<RegisterSet>
{
};

DPointeredNoCopy(RegisterSet);

RegisterSet::RegisterSet() {}

RegisterSet::~RegisterSet() {}

RegVal RegisterSet::reg(const std::string &name) const
{
	auto regs = registers();
	for (const auto &reg : regs)
	{
		if (reg.isNameMatch(name))
			return reg.value();
	}
	throw UnknownRegisterError("unknown register " + name);
}
