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
#include "memory.hpp"

namespace Emuballs
{

static const int WORD_SIZE = 4;

Page::Page(memsize size)
{
	bytes.resize(size);
}

const uint8_t &Page::operator[](memsize index) const
{
	if (index >= bytes.size())
	{
		throw std::out_of_range("index outsize page size");
	}
	return bytes[index];
}

const std::vector<uint8_t> &Page::contents() const
{
	return bytes;
}

memsize Page::size() const
{
	return bytes.size();
}

}

//////////////////////////////////////////////////////////////////////

DClass<Emuballs::Memory>
{
public:
	Emuballs::memsize size;
	Emuballs::memsize pageSize;
	mutable std::map<Emuballs::memsize, Emuballs::Page> pages;
};

DPointered(Emuballs::Memory);

namespace Emuballs
{

Memory::Memory(memsize totalSize, memsize pageSize)
{
	d->size = totalSize;
	d->pageSize = pageSize;
}

std::vector<memsize> Memory::allocatedPages() const
{
	std::vector<memsize> pages;
	for (auto &pair : d->pages)
		pages.push_back(pair.first);
	return pages;
}

std::vector<uint8_t> Memory::chunk(memsize address, memsize length) const
{
	memsize currentPageAddress = pageAddress(address);
	std::vector<uint8_t> bytes(length);
	for (; currentPageAddress < length; currentPageAddress += d->pageSize)
	{
		memsize insertCount = std::min(length - currentPageAddress, d->pageSize);
		const Page &p = page(address);
		const std::vector<uint8_t> &pageBytes = p.contents();
		bytes.insert(bytes.end(), pageBytes.begin(), pageBytes.begin() + insertCount);
	}
	return bytes;
}

void Memory::putByte(memsize address, uint8_t value)
{
	page(address)[pageOffset(address)] = value;
}

uint8_t Memory::byte(memsize address) const
{
	return page(address)[pageOffset(address)];
}

void Memory::putWord(memsize address, uint32_t value)
{
	Page *p = &page(address);
	memsize offset = pageOffset(address);
	for (int i = 0; i < WORD_SIZE; ++i, ++offset, ++address)
	{
		if (offset >= p->size())
		{
			p = &page(address);
			offset = pageOffset(address);
		}
		(*p)[offset] = static_cast<uint8_t>(value >> (8 * i));
	}
}

uint32_t Memory::word(memsize address) const
{
	const Page *p = &page(address);
	memsize offset = pageOffset(address);
	uint32_t value = 0;
	for (int i = 0; i < WORD_SIZE; ++i, ++offset, ++address)
	{
		if (offset >= p->size())
		{
			p = &page(address);
			offset = pageOffset(address);
		}
		value |= static_cast<uint32_t>((*p)[offset]) << (8 * i);
	}
	return value;
}

const Page &Memory::page(memsize address) const
{
	if (address >= d->size)
	{
		throw std::out_of_range("address too big");
	}
	memsize page = pageAddress(address);
	if (d->pages.find(page) == d->pages.end())
	{
		d->pages[page] = Page(d->pageSize);
	}
	return d->pages[page];
}

memsize Memory::pageAddress(memsize memAddress) const
{
	return (memAddress / d->pageSize) * d->pageSize;
}

memsize Memory::pageOffset(memsize memAddress) const
{
	return memAddress % d->pageSize;
}

memsize Memory::size() const
{
	return d->size;
}

}
