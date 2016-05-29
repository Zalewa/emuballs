//------------------------------------------------------------------------------
// dptr.h
//------------------------------------------------------------------------------
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//
//------------------------------------------------------------------------------
// Copyright (C) 2015 Braden "Blzut3" Obrzut <admin@maniacsvault.net>
// Repurposed from Qt pointers to C++11 by Zalewa <zalewapl@gmail.com>, 2016
//------------------------------------------------------------------------------
#ifndef id91044119_93C0_4719_829DE83410DDE660
#define id91044119_93C0_4719_829DE83410DDE660

#include <memory>

/**
 * Private data structure for your class T. This needs to be specialized.
 */
template<class T> class PrivData;
/**
 * Syntactic sugar for specializing PrivData. Use is DClass<T>.
 */
#define DClass template<> class PrivData

/**
 * RAII wrapper for implementing d-pointers. The resulting object can be copied
 * through normal copy construction and assignment. Additionally it is
 * compatible with all implicitly generated functions. (Note that implicitly
 * generated functions are inlined so adding one to your class later will
 * break your ABI.)
 *
 * Use in your class declaration is fairly standard except that T should be
 * the type of the class it is contained in. In your implementation file you
 * will need to specialize PrivData<T> (which can be done with the DClass<T>
 * macro). Additionally every member of this class must be specialized in the
 * implementation file, which will usually be done with DPointered(T) or
 * DPointeredNoCopy(T). The implementation of these functions is fairly trivial
 * so they could be specialized manually as a way out of the previously
 * mentioned ABI breakage.
 */
template<class T>
class DPtr : public std::unique_ptr<PrivData<T> >
{
public:
	DPtr();
	DPtr(const DPtr &other);
	const DPtr &operator=(const DPtr &other);
	~DPtr();
};

/**
 * Specializes DPtr<T> for non-copyable DClass.
 */
#define DPointeredNoCopy(cls)  \
template<> \
DPtr<cls>::DPtr() : std::unique_ptr<PrivData<cls> >(new PrivData<cls>) {} \
template<> \
DPtr<cls>::~DPtr() {}

/**
 * Standard method for specializing DPtr<T>.
 */
#define DPointered(cls) DPointeredNoCopy(cls) \
template<> \
DPtr<cls>::DPtr(const DPtr<cls> &other) : std::unique_ptr<PrivData<cls> >(new PrivData<cls>) \
{ \
	*(this->get()) = *(other.get()); \
} \
template<> \
const DPtr<cls> &DPtr<cls>::operator=(const DPtr<cls> &other) \
{ \
	if(this->get() != other.get()) \
		*(this->get()) = *(other.get()); \
	return *this; \
}

#endif
