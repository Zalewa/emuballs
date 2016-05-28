#include "memory.hpp"

namespace Machine
{

static const int WORD_SIZE = 4;

Page::Page(size_t size)
{
	bytes.resize(size);
}

const uint8_t &Page::operator[](size_t index) const
{
	if (index >= bytes.size())
	{
		throw std::out_of_range("index outsize page size");
	}
	return bytes[index];
}

size_t Page::size() const
{
	return bytes.size();
}

//////////////////////////////////////////////////////////////////////

Memory::Memory(size_t totalSize, size_t pageSize)
{
	this->size = totalSize;
	this->pageSize = pageSize;
}

void Memory::putByte(uint32_t address, uint8_t value)
{
	page(address)[pageOffset(address)] = value;
}

uint8_t Memory::byte(uint32_t address) const
{
	return page(address)[pageOffset(address)];
}

void Memory::putWord(uint32_t address, uint32_t value)
{
	Page *p = &page(address);
	uint32_t offset = pageOffset(address);
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

uint32_t Memory::word(uint32_t address) const
{
	const Page *p = &page(address);
	uint32_t offset = pageOffset(address);
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

const Page &Memory::page(uint32_t address) const
{
	if (address >= size)
	{
		throw std::out_of_range("address too big");
	}
	uint32_t page = pageAddress(address);
	if (pages.find(page) == pages.end())
	{
		pages[page] = Page(pageSize);
	}
	return pages[page];
}

uint32_t Memory::pageAddress(uint32_t memAddress) const
{
	return (memAddress / pageSize) * pageSize;
}

uint32_t Memory::pageOffset(uint32_t memAddress) const
{
	return memAddress % pageSize;
}

}
