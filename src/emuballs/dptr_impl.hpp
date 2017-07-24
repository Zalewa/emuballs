//------------------------------------------------------------------------------
// dptr_impl.hpp
//------------------------------------------------------------------------------
//
// This file is part of Emuballs.
//
// Emuballs is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Emuballs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Emuballs.  If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------
// Copyright (C) 2015 Braden "Blzut3" Obrzut <admin@maniacsvault.net>
// Repurposed from Qt pointers to C++11 by Zalewa <zalewapl@gmail.com>, 2016
//------------------------------------------------------------------------------
#pragma once

#include "emuballs/dptr.hpp"

/**
 * Syntactic sugar for specializing PrivData. Use is DClass<T>.
 */
#define DClass template<> class PrivData


/**
 * Specializes DPtr<T> for non-copyable DClass.
 */
#define DPointeredNoCopy(cls)  \
template<> \
Emuballs::DPtr<cls>::DPtr() \
	: std::unique_ptr<Emuballs::PrivData<cls> >(new Emuballs::PrivData<cls>) {} \
template<> \
Emuballs::DPtr<cls>::~DPtr() {}

/**
 * Standard method for specializing DPtr<T>.
 */
#define DPointered(cls) DPointeredNoCopy(cls) \
template<> \
Emuballs::DPtr<cls>::DPtr(const Emuballs::DPtr<cls> &other) \
	: std::unique_ptr<Emuballs::PrivData<cls> >(new Emuballs::PrivData<cls>) \
{ \
	*(this->get()) = *(other.get()); \
} \
template<> \
const Emuballs::DPtr<cls> &Emuballs::DPtr<cls>::operator=(const Emuballs::DPtr<cls> &other) \
{ \
	if(this->get() != other.get()) \
		*(this->get()) = *(other.get()); \
	return *this; \
}
