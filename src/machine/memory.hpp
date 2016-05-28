#pragma once

#include <cstdint>
#include <map>
#include <vector>

namespace Machine
{
class Page
{
public:
	Page(size_t size = 0);

	const uint8_t &operator[](size_t index) const;
	uint8_t &operator[](size_t index)
	{
		return const_cast<uint8_t&>( static_cast<const Page&>(*this)[index] );
	}

	size_t size() const;

private:
	std::vector<uint8_t> bytes;
};

class Memory
{
public:
	Memory(size_t totalSize = SIZE_MAX, size_t pageSize = 4096);

	void putByte(uint32_t address, uint8_t value);
	uint8_t byte(uint32_t address) const;

	void putWord(uint32_t address, uint32_t value);
	uint32_t word(uint32_t address) const;

private:
	const Page &page(uint32_t address) const;
	Page &page(uint32_t address)
	{
		return const_cast<Page&>( static_cast<const Memory&>(*this).page(address) );
	}

	uint32_t pageAddress(uint32_t memAddress) const;
	uint32_t pageOffset(uint32_t memAddress) const;

	size_t size;
	size_t pageSize;
	mutable std::map<uint32_t, Page> pages;
};
}
