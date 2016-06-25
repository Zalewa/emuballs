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

#include <algorithm>
#include <deque>
#include <functional>
#include <stdexcept>

namespace Emuballs
{

namespace Arm
{

template<class T> T bitsToVal(const std::deque<bool> &bits)
{
	T retval = 0;
	std::for_each(bits.begin(), bits.end(), [&retval](bool bit) {
			retval <<= 1;
			retval |= bit;
		});
	return retval;
}

template<class T> std::deque<bool> valToBits(const T &value)
{
	T valueCopy = value;
	std::deque<bool> bits;
	for (auto i = 0U; i < sizeof(T) * 8; ++i)
	{
		bits.push_front(valueCopy & 1);
		valueCopy >>= 1;
	}
	return bits;
}

template<class T> T logicalLeft(const T &value, unsigned amount, bool *carry = nullptr)
{
	std::deque<bool> bits = valToBits<T>(value);
	for (auto i = 0U; i < amount; ++i)
	{
		if (carry)
			*carry = bits.front();
		bits.pop_front();
		bits.push_back(false);
	}
	return bitsToVal<T>(bits);
}

template<class T> T logicalRight(T value, unsigned amount, bool *carry = nullptr)
{
	std::deque<bool> bits = valToBits<T>(value);
	for (auto i = 0U; i < amount; ++i)
	{
		if (carry)
			*carry = bits.back();
		bits.pop_back();
		bits.push_front(false);
	}
	return bitsToVal<T>(bits);
}

template<class T> T arithmeticRight(T value, unsigned amount, bool *carry = nullptr)
{
	std::deque<bool> bits = valToBits<T>(value);
	for (auto i = 0U; i < amount; ++i)
	{
		if (carry)
			*carry = bits.back();
		bits.pop_back();
		bits.push_front(bits.front());
	}
	return bitsToVal<T>(bits);
}

template<class T> T rotateRight(T value, unsigned amount, bool *carry = nullptr)
{
	std::deque<bool> bits = valToBits<T>(value);
	for (auto i = 0U; i < amount; ++i)
	{
		bool bit = bits.back();
		if (carry)
			*carry = bit;
		bits.pop_back();
		bits.push_front(bit);
	}
	return bitsToVal<T>(bits);
}

template<class T> T rotateRightExtended(T value, bool carryIn, bool *carryOut)
{
	std::deque<bool> bits = valToBits<T>(value);
	if (carryOut)
		*carryOut = bits.back();
	bits.pop_back();
	bits.push_front(carryIn);
	return bitsToVal<T>(bits);
}

enum class ShiftType : int
{
	lsl = 0,
	lsr = 1,
	asr = 2,
	ror = 3
};

template<class T> using bitshift = std::function<T(T, int, bool*)>;

template<class T> bitshift<T> shifter(int signature)
{
	switch (static_cast<ShiftType>(signature))
	{
	case ShiftType::lsl:
		return logicalLeft<T>;
	case ShiftType::lsr:
		return logicalRight<T>;
	case ShiftType::asr:
		return arithmeticRight<T>;
	case ShiftType::ror:
		return rotateRight<T>;
	default:
		throw std::domain_error("bad shift signature " + std::to_string(signature));
	}
}


}

}
