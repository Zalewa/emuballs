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

void Page::setContents(memsize offset, const std::vector<uint8_t> &bytes)
{
	if (offset + bytes.size() > size())
		throw std::out_of_range("contents must be within page size");
	else if (offset == 0 && bytes.size() == size())
		this->bytes = bytes;
	else
	{
		for (size_t idx = 0; idx < bytes.size(); ++idx)
			this->bytes[offset + idx] = bytes[idx];
	}
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
	Emuballs::Page falsePage;

	const Emuballs::Page &page(Emuballs::memsize address) const
	{
		if (address >= size)
			throw std::out_of_range("address too big");
		Emuballs::memsize page = pageAddress(address);
		if (!isPageAllocated(page))
			allocatePage(page);
		return pages[page];
	}


	Emuballs::Page &page(Emuballs::memsize address)
	{
		return const_cast<Emuballs::Page&>( static_cast<const PrivData<Emuballs::Memory> &>(*this).page(address) );
	}

	Emuballs::memsize pageAddress(Emuballs::memsize memAddress) const
	{
		return (memAddress / pageSize) * pageSize;
	}

	Emuballs::memsize pageOffset(Emuballs::memsize memAddress) const
	{
		return memAddress % pageSize;
	}

	bool isPageAllocated(Emuballs::memsize address) const
	{
		return pages.find(pageAddress(address)) != pages.end();
	}

	void allocatePage(Emuballs::memsize address) const
	{
		pages[pageAddress(address)] = Emuballs::Page(pageSize);
	}
};

DPointered(Emuballs::Memory);

namespace Emuballs
{

Memory::Memory(memsize totalSize, memsize pageSize)
{
	d->size = totalSize;
	d->pageSize = pageSize;
	d->falsePage = Page(pageSize);
}

std::vector<memsize> Memory::allocatedPages() const
{
	std::vector<memsize> pages;
	for (auto &pair : d->pages)
		pages.push_back(pair.first);
	return pages;
}

memsize Memory::putChunk(memsize address, const std::vector<uint8_t> &chunk)
{
	memsize totalInsertCount = 0;
	memsize currentPageAddress = d->pageAddress(address);
	memsize currentPageOffset = d->pageOffset(address);
	for (memsize offset = 0;
		 currentPageAddress < size() && offset < chunk.size();
		 currentPageAddress += d->pageSize)
	{
		memsize insertCount = std::min(d->pageSize - currentPageOffset, chunk.size() - offset);
		totalInsertCount += insertCount;
		Page &p = d->page(currentPageAddress);
		p.setContents(currentPageOffset, std::vector<uint8_t>(
				chunk.begin() + offset,
				chunk.begin() + offset + insertCount));
		offset += insertCount;
		currentPageOffset = 0;
	}
	return totalInsertCount;
}

std::vector<uint8_t> Memory::chunk(memsize address, memsize length) const
{
	memsize currentPageAddress = d->pageAddress(address);
	memsize currentPageOffset = d->pageOffset(address);
	std::vector<uint8_t> bytes;
	if (length > d->pageSize)
		bytes.reserve(length);
	memsize remainingLength = length;
	for (; currentPageAddress < size() && remainingLength > 0;
		 currentPageAddress += d->pageSize)
	{
		memsize insertCount = std::min(remainingLength, d->pageSize - currentPageOffset);
		const Page &p = d->isPageAllocated(currentPageAddress) ?
			d->page(currentPageAddress) :
			d->falsePage;
		const std::vector<uint8_t> &pageBytes = p.contents();
		bytes.insert(bytes.end(),
			pageBytes.begin() + currentPageOffset,
			pageBytes.begin() + currentPageOffset + insertCount);
		remainingLength -= insertCount;
		currentPageOffset = 0;
	}
	return bytes;
}

void Memory::putByte(memsize address, uint8_t value)
{
	d->page(address)[d->pageOffset(address)] = value;
}

uint8_t Memory::byte(memsize address) const
{
	return d->page(address)[d->pageOffset(address)];
}

void Memory::putWord(memsize address, uint32_t value)
{
	Page *p = &d->page(address);
	memsize offset = d->pageOffset(address);
	for (int i = 0; i < WORD_SIZE; ++i, ++offset, ++address)
	{
		if (offset >= p->size())
		{
			p = &d->page(address);
			offset = d->pageOffset(address);
		}
		(*p)[offset] = static_cast<uint8_t>(value >> (8 * i));
	}
}

uint32_t Memory::word(memsize address) const
{
	const Page *p = &d->page(address);
	memsize offset = d->pageOffset(address);
	uint32_t value = 0;
	for (int i = 0; i < WORD_SIZE; ++i, ++offset, ++address)
	{
		if (offset >= p->size())
		{
			p = &d->page(address);
			offset = d->pageOffset(address);
		}
		value |= static_cast<uint32_t>((*p)[offset]) << (8 * i);
	}
	return value;
}

memsize Memory::pageSize() const
{
	return d->pageSize;
}

memsize Memory::size() const
{
	return d->size;
}

}
