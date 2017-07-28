/*
 * Copyright 2017 Zalewa <zalewapl@gmail.com>.
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

#include <array>
#include <cstdint>

namespace Emuballs
{

/**
 * ArrayQueue is like a std::queue but with statically limited
 * amount of elements.
 *
 * This queue avoids dynamic memory allocation by storing
 * elements in an array and overwriting unused ones when pushing.
 *
 * This is a FIFO queue.
 *
 * The template arguments denote:
 *
 * @param T
 *     Type of elements that will be stored in the queue.
 * @param maxSize
 *     Maximum size of the queue.
 */
template<typename T, size_t maxSize>
class ArrayQueue
{
public:
	/**
	 * Empties the queue, resetting it to a fresh state.
	 */
	void clear()
	{
		popIndex = 0;
		pushIndex = 0;
		_size = 0;
	}

	/**
	 * Push element to the queue at the end.
	 *
	 * @throw std::length_error if maxSize is already reached.
	 */
	void push(T value)
	{
		if (_size >= maxSize)
			throw std::length_error("queue is full");
		array[pushIndex] = value;
		++pushIndex;
		if (pushIndex >= maxSize)
			pushIndex = 0;
		++_size;
	}

	/**
	 * Pop element from the start of the queue.
	 *
	 * @throw std::length_error there are no elements on the queue.
	 * @return Element of type T.
	 */
	T pop()
	{
		if (_size == 0)
			throw std::length_error("queue is empty");
		T value = array[popIndex];
		++popIndex;
		if (popIndex >= maxSize)
			popIndex = 0;
		--_size;
		return value;
	}

	/**
	 * @return Current amount of elements in th queue.
	 */
	size_t size() const
	{
		return _size;
	}

private:
	std::array<T, maxSize> array;
	size_t popIndex = 0;
	size_t pushIndex = 0;
	size_t _size = 0;
};

}
