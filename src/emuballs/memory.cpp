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

#include <algorithm>
#include <list>

#ifdef EMUBALLS_DEBUGS
#include <iostream>
#endif

namespace Emuballs
{

struct MemObserveZone
{
	memobserver_id id;
	memsize address;
	memsize length;
	memobserver observer;
	Access events;
	bool blocked = false;

	memsize end() const
	{
		validate();
		return address + length - 1;
	}

	bool isRead() const
	{
		return static_cast<bool>(events & Access::Read);
	}

	bool isWrite() const
	{
		return static_cast<bool>(events & Access::Write);
	}

	bool isInZone(memsize address) const
	{
		validate();
		return address >= this->address && address < this->address + length;
	}

	bool isRangeColliding(memsize from, memsize to) const
	{
		validate();
		auto lesser = std::min(from, to);
		auto greater = std::max(from, to);
		auto end = this->end();
		return (lesser >= address && greater <= end)
			|| (lesser <= address && greater >= address)
			|| (lesser <= end && greater >= end);
	}

private:
	void validate() const
	{
		if (length == 0)
			throw std::runtime_error("memory observer with 0 length");
	}
};


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
	Emuballs::memobserver_id observeId;
	std::list<Emuballs::MemObserveZone> observers;

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

	void execObservers(Emuballs::memsize address, Emuballs::Access event)
	{
		for (Emuballs::MemObserveZone &observer : this->observers)
		{
			#ifdef EMUBALLS_DEBUGS
			if (!observer.blocked) {
				std::cerr << "execObservers " << std::hex << address << " " << static_cast<int>(event) << " in da zone? " << observer.isInZone(address) << " events? " << static_cast<int>(observer.events & event) << " addr=" << observer.address << std::dec << " length=" << observer.length << std::endl;
			}
			#endif
			if (!observer.blocked && (observer.events & event) != 0
				&& observer.isInZone(address))
			{
				observer.observer(address, event);
			}
		}
	}

	void execObserversInRange(Emuballs::memsize address, Emuballs::memsize length,
		Emuballs::Access event)
	{
		for (Emuballs::MemObserveZone &observer : this->observers)
		{
			if (!observer.blocked && (observer.events & event) != 0
				&& observer.isRangeColliding(address, length))
			{
				observer.observer(address, event);
			}
		}
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
	d->observeId = 1; // 0 is special; it should denote no observer
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
	d->execObserversInRange(address, totalInsertCount, Access::Write);
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
	d->execObserversInRange(address, bytes.size(), Access::Read);
	return bytes;
}

void Memory::putByte(memsize address, uint8_t value)
{
	d->page(address)[d->pageOffset(address)] = value;
	d->execObservers(address, Access::Write);
}

uint8_t Memory::byte(memsize address) const
{
	d->execObservers(address, Access::Read);
	return d->page(address)[d->pageOffset(address)];
}

void Memory::putWord(memsize address, uint32_t value)
{
	Page *p = &d->page(address);
	memsize offset = d->pageOffset(address);
	memsize currentAddress = address;
	for (int i = 0; i < WORD_SIZE; ++i, ++offset, ++currentAddress)
	{
		if (offset >= p->size())
		{
			p = &d->page(currentAddress);
			offset = d->pageOffset(currentAddress);
		}
		(*p)[offset] = static_cast<uint8_t>(value >> (8 * i));
	}
	d->execObservers(address, Access::Write);
}

uint32_t Memory::word(memsize address) const
{
	const Page *p = &d->page(address);
	memsize offset = d->pageOffset(address);
	uint32_t value = 0;
	memsize currentAddress = address;
	for (int i = 0; i < WORD_SIZE; ++i, ++offset, ++currentAddress)
	{
		if (offset >= p->size())
		{
			p = &d->page(currentAddress);
			offset = d->pageOffset(currentAddress);
		}
		value |= static_cast<uint32_t>((*p)[offset]) << (8 * i);
	}
	d->execObservers(address, Access::Read);
	return value;
}

void Memory::putDword(memsize address, uint64_t value)
{
	putWord(address, static_cast<uint32_t>(value & 0xffffffff));
	putWord(address + sizeof(uint32_t), static_cast<uint32_t>((value >> 32) & 0xffffffff));
}

uint64_t Memory::dword(memsize address) const
{
	uint64_t low = word(address);
	uint64_t high = word(address + sizeof(uint32_t));
	return (high << 32) | low;
}

memsize Memory::pageSize() const
{
	return d->pageSize;
}

memsize Memory::size() const
{
	return d->size;
}

memobserver_id Memory::observe(memsize address, memsize length, memobserver observer, Access events)
{
	memobserver_id id = d->observeId++;
	MemObserveZone observerDescriptor {
		.id = id,
		.address = address,
		.length = length,
		.observer = observer,
		.events = events
	};
	d->observers.emplace_back(observerDescriptor);
	return id;
}

void Memory::blockObservation(memobserver_id id, bool block)
{
	auto it = std::find_if(d->observers.begin(), d->observers.end(),
		[id](MemObserveZone& zone){ return zone.id == id; });
	if (it != d->observers.end())
		it->blocked = block;
	else
		throw std::runtime_error("no observer");
}

void Memory::unobserve(memobserver_id id)
{
	for (auto it = d->observers.begin(); it != d->observers.end(); ++it)
	{
		if (it->id == id)
		{
			d->observers.erase(it);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////

MemoryStreamReader::MemoryStreamReader(const Memory &memory, memsize startOffset)
	: memory(memory), _offset(startOffset)
{
}

uint32_t MemoryStreamReader::readUint32()
{
	uint32_t val = memory.word(_offset);
	_offset += sizeof(uint32_t);
	return val;
}

uint64_t MemoryStreamReader::readUint64()
{
	uint64_t val = memory.dword(_offset);
	_offset += sizeof(uint64_t);
	return val;
}

void MemoryStreamReader::skip(memsize amount)
{
	_offset += amount;
}

//////////////////////////////////////////////////////////////////////

MemoryStreamWriter::MemoryStreamWriter(Memory &memory, memsize startOffset)
	: memory(memory), _offset(startOffset)
{
}

void MemoryStreamWriter::writeUint32(uint32_t val)
{
	memory.putWord(_offset, val);
	_offset += sizeof(uint32_t);
}

void MemoryStreamWriter::writeUint64(uint64_t val)
{
	memory.putDword(_offset, val);
	_offset += sizeof(uint64_t);
}

void MemoryStreamWriter::skip(memsize amount)
{
	_offset += amount;
}

}
