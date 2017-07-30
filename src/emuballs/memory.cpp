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
#include "memory_impl.hpp"

#include <algorithm>
#include <list>
#include <map>

#include "dptr_impl.hpp"

namespace Emuballs
{

struct MemObserveZone
{
	memobserver_id id;
	memsize address;
	memsize length;
	memobserver observer;
	Access events;

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

std::vector<uint8_t> &Page::contents()
{
	return bytes;
}

void Page::setContents(memsize offset, const std::vector<uint8_t> &bytes)
{
	setContents(offset, bytes.begin(), bytes.end());
}

memsize Page::size() const
{
	return bytes.size();
}

}

//////////////////////////////////////////////////////////////////////

namespace Emuballs
{
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
		auto it = pages.find(page);
		if (it != pages.end())
		{
			return it->second;
		}
		else
		{
			allocatePage(page);
			return pages[page];
		}
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
			if ((observer.events & event) != 0
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
			if ((observer.events & event) != 0
				&& observer.isRangeColliding(address, length))
			{
				observer.observer(address, event);
			}
		}
	}
};

DPointered(Emuballs::Memory);

Memory::Memory(memsize totalSize, memsize pageSize)
{
	d->size = totalSize;
	d->pageSize = pageSize;
	d->falsePage = Page(pageSize);
	d->observeId = 1; // 0 is special; it should denote no observer
}

Memory::~Memory()
{
}

std::vector<memsize> Memory::allocatedPages() const
{
	std::vector<memsize> pages;
	for (auto &pair : d->pages)
		pages.push_back(pair.first);
	return pages;
}

void Memory::execObservers(memsize address, memsize length, Access events)
{
	if (length <= 1)
	{
		d->execObservers(address, events);
	}
	else
	{
		d->execObserversInRange(address, length, events);
	}
}

memsize Memory::putChunk(memsize address, const std::vector<uint8_t> &chunk)
{
	return putChunk(address, chunk.data(), chunk.size());
}

memsize Memory::putChunk(memsize address, const uint8_t *begin, memsize length)
{
	memsize totalInsertCount = 0;
	memsize currentPageAddress = d->pageAddress(address);
	memsize currentPageOffset = d->pageOffset(address);
	for (memsize offset = 0;
		 currentPageAddress < size() && offset < length;
		 currentPageAddress += d->pageSize)
	{
		memsize insertCount = std::min(d->pageSize - currentPageOffset, length - offset);
		totalInsertCount += insertCount;
		Page &p = d->page(currentPageAddress);
		p.setContents(currentPageOffset,
			begin + offset,
			begin + offset + insertCount);
		offset += insertCount;
		currentPageOffset = 0;
	}
	return totalInsertCount;
}

std::vector<uint8_t> Memory::chunk(memsize address, memsize length) const
{
	uint8_t *buffer = new uint8_t[length];
	auto read = chunk(address, length, buffer);
	std::vector<uint8_t> bytes;
	bytes.assign(buffer, buffer + read);
	delete [] buffer;
	return bytes;
}

memsize Memory::chunk(memsize address, memsize length, uint8_t *begin) const
{
	if (address >= size())
		return 0;
	if (address + length > size())
		length = size() - address;

	memsize currentPageAddress = d->pageAddress(address);
	memsize currentPageOffset = d->pageOffset(address);
	memsize remainingLength = length;
	while (remainingLength > 0)
	{
		memsize insertCount = std::min(remainingLength, d->pageSize - currentPageOffset);
		const Page &p = d->isPageAllocated(currentPageAddress) ?
			d->page(currentPageAddress) :
			d->falsePage;
		const std::vector<uint8_t> &pageBytes = p.contents();
		std::copy(
			pageBytes.cbegin() + currentPageOffset,
			pageBytes.cbegin() + currentPageOffset + insertCount,
			begin);
		begin += insertCount;
		remainingLength -= insertCount;
		currentPageAddress += d->pageSize;
		currentPageOffset = 0;
	}
	return length;
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
}

uint32_t Memory::word(memsize address) const
{
	const Page *p = &d->page(address);
	memsize offset = d->pageOffset(address);
	uint32_t value = 0;
	if (offset + WORD_SIZE < p->size())
	{
		// We can optimize things a bit if word is all on the same page.
		#ifdef EMUBALLS_BIG_ENDIAN
		#error("big endian not supported")
		#endif
		auto &memContents = p->contents();
		std::copy(
			memContents.begin() + offset,
			memContents.begin() + offset + WORD_SIZE,
			reinterpret_cast<uint8_t*>(&value));
	}
	else
	{
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
	}
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

uint8_t *Memory::ptr(memsize address)
{
	return d->page(address).contents().data() + d->pageOffset(address);
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

//////////////////////////////////////////////////////////////////////

TrackedMemory::TrackedMemory(Memory &memory)
	: memory(memory)
{
}

memsize TrackedMemory::putChunk(memsize address, const std::vector<uint8_t> &chunk)
{
	auto ret = memory.putChunk(address, chunk);
	memory.execObservers(address, ret, Access::Write);
	return ret;
}

memsize TrackedMemory::putChunk(memsize address, const uint8_t *begin, memsize length)
{
	auto ret = memory.putChunk(address, begin, length);
	memory.execObservers(address, ret, Access::Write);
	return ret;
}

std::vector<uint8_t> TrackedMemory::chunk(memsize address, memsize length) const
{
	memory.execObservers(address, length, Access::PreRead);
	auto ret = memory.chunk(address, length);
	memory.execObservers(address, ret.size(), Access::Read);
	return ret;
}

memsize TrackedMemory::chunk(memsize address, memsize length, uint8_t *begin) const
{
	memory.execObservers(address, length, Access::PreRead);
	auto ret = memory.chunk(address, length, begin);
	memory.execObservers(address, ret, Access::Read);
	return ret;
}

void TrackedMemory::putByte(memsize address, uint8_t value)
{
	memory.putByte(address, value);
	memory.execObservers(address, 0, Access::Write);
}

uint8_t TrackedMemory::byte(memsize address) const
{
	memory.execObservers(address, 0, Access::PreRead);
	auto ret = memory.byte(address);
	memory.execObservers(address, 0, Access::Read);
	return ret;
}

void TrackedMemory::putWord(memsize address, uint32_t value)
{
	memory.putWord(address, value);
	memory.execObservers(address, 0, Access::Write);
}

uint32_t TrackedMemory::word(memsize address) const
{
	memory.execObservers(address, 0, Access::PreRead);
	auto ret = memory.word(address);
	memory.execObservers(address, 0, Access::Read);
	return ret;
}

void TrackedMemory::putDword(memsize address, uint64_t value)
{
	memory.putDword(address, value);
	memory.execObservers(address, 0, Access::Write);
}

uint64_t TrackedMemory::dword(memsize address) const
{
	memory.execObservers(address, 0, Access::PreRead);
	auto ret = memory.dword(address);
	memory.execObservers(address, 0, Access::Read);
	return ret;
}

}
