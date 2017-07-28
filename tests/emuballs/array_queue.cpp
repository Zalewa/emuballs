/*
 * Copyright 2017 Zalewa <zalewapl@gmail.com>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#define BOOST_TEST_MODULE array_queue
#include <boost/test/unit_test.hpp>
#include "src/emuballs/array_queue.hpp"

BOOST_AUTO_TEST_CASE(push_one)
{
	Emuballs::ArrayQueue<int, 1> q;
	BOOST_CHECK_EQUAL(0, q.size());
	q.push(1);
	BOOST_CHECK_EQUAL(1, q.size());
}

BOOST_AUTO_TEST_CASE(push_at_zero_queue)
{
	Emuballs::ArrayQueue<int, 0> q;
	BOOST_CHECK_EQUAL(0, q.size());
	BOOST_CHECK_NO_THROW(q.size());
	BOOST_CHECK_THROW(q.push(0xcafe), std::length_error);
}

BOOST_AUTO_TEST_CASE(push_it_to_the_limit)
{
	Emuballs::ArrayQueue<int, 10> q;
	for (int i = 1; i <= 10; ++i)
		q.push(i * 3);
	BOOST_CHECK_EQUAL(10, q.size());
	BOOST_CHECK_THROW(q.push(33), std::length_error);
}

BOOST_AUTO_TEST_CASE(push_then_pop)
{
	Emuballs::ArrayQueue<int, 5> q;
	q.push(2);
	q.push(4);
	q.push(7);
	BOOST_CHECK_EQUAL(2, q.pop());
	BOOST_CHECK_EQUAL(4, q.pop());
	BOOST_CHECK_EQUAL(7, q.pop());
}

BOOST_AUTO_TEST_CASE(pop_nothing)
{
	Emuballs::ArrayQueue<int, 5> q;
	BOOST_CHECK_EQUAL(0, q.size());
	BOOST_CHECK_THROW(q.pop(), std::length_error);
}

BOOST_AUTO_TEST_CASE(pop_zero_length)
{
	Emuballs::ArrayQueue<int, 0> q;
	BOOST_CHECK_THROW(q.pop(), std::length_error);
}

BOOST_AUTO_TEST_CASE(push_pop_cycle)
{
	Emuballs::ArrayQueue<int, 2> q;
	q.push(13);
	q.push(18);
	BOOST_CHECK_EQUAL(2, q.size());
	BOOST_CHECK_EQUAL(13, q.pop());
	BOOST_CHECK_EQUAL(1, q.size());
	q.push(27);
	BOOST_CHECK_EQUAL(2, q.size());
	BOOST_CHECK_EQUAL(18, q.pop());
	BOOST_CHECK_EQUAL(1, q.size());
	BOOST_CHECK_EQUAL(27, q.pop());
	BOOST_CHECK_EQUAL(0, q.size());
	q.push(-1);
	q.push(-2);
	BOOST_CHECK_EQUAL(2, q.size());
	BOOST_CHECK_EQUAL(-1, q.pop());
	BOOST_CHECK_EQUAL(1, q.size());
	BOOST_CHECK_EQUAL(-2, q.pop());
	BOOST_CHECK_EQUAL(0, q.size());
}
