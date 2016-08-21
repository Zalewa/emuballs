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
#include <list>
#include <string>
#include <map>

namespace Emuballs
{

class RegVal;

class EMUBALLS_API NamedRegister
{
public:
	NamedRegister();
	NamedRegister(const std::string &name, const RegVal &val);
	NamedRegister(const std::list<std::string> &names, const RegVal &val);
	virtual ~NamedRegister();

	std::list<std::string> names() const;
	RegVal value() const;

	bool isNameMatch(const std::string &name) const;

private:
	DPtr<NamedRegister> d;
};

/**
 * CPU register set collection; bases itself on unique names.
 */
class EMUBALLS_API RegisterSet
{
public:
	RegisterSet();
	RegisterSet(const RegisterSet &other) = delete;
	RegisterSet &operator=(const RegisterSet &other) = delete;
	virtual ~RegisterSet();

	/**
	 * Mapping of all registers and their names.
	 */
	virtual std::list<NamedRegister> registers() const = 0;

	/**
	 * Set value of specific register.
	 *
	 * @throws UnkownRegisterError If machine has no register of specified name.
	 */
	virtual void setReg(const std::string &name, const RegVal &val) = 0;

	/**
	 * Get value of specific register.
	 *
	 * @throws UnkownRegisterError If machine has no register of specified name.
	 */
	virtual RegVal reg(const std::string &name) const;

private:
	DPtr<RegisterSet> d;
};

}
