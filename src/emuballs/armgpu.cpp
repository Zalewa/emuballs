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
#include "armgpu.hpp"

#include "canvas.hpp"
#include "color.hpp"
#include <functional>
#include <iostream>
using namespace Emuballs;
using namespace Emuballs::Arm;

namespace Emuballs::Arm
{
enum class StatusBit : uint32_t
{
	ReadReady = 1u << 30,
	WriteReady = 1u << 31
};

struct Mail
{
	static const uint8_t MIN_CHANNEL = 0;
	static const uint8_t NUM_CHANNELS = 16;
	static const uint32_t CHANNEL_MASK = 0b1111;

	uint8_t channel;
	uint32_t message;

	Mail() {};

	Mail(uint32_t bits)
	{
		channel = bits & CHANNEL_MASK;
		message = bits & (~CHANNEL_MASK);
	}

	operator uint32_t() const
	{
		return message | static_cast<uint32_t>(channel);
	}
};

struct Mailbox
{
	/** Receiving mail. */
	uint32_t read;
	uint32_t reserved[4];
	/** Receive without retrieving; value of `read` is written to memory. */
	uint32_t poll;
	/** Sender information. */
	uint32_t sender;
	uint32_t status;
	uint32_t configuration;
	/** Sending mail. */
	uint32_t write;

	bool isReadReady() const
	{
		return (status & readyBit(StatusBit::ReadReady, true)) != 0;
	}

	void readReady(bool ready)
	{
		status &= readyBit(StatusBit::ReadReady, ready);
	}

	bool isWriteReady() const
	{
		return (status & readyBit(StatusBit::WriteReady, true)) != 0;
	}

	void writeReady(bool ready)
	{
		status &= readyBit(StatusBit::WriteReady, ready);
	}

private:
	static uint32_t readyBit(StatusBit bit, bool ready)
	{
		return ready ? ~static_cast<uint32_t>(bit) : static_cast<uint32_t>(bit);
	}
};
}

DClass<Gpu>
{
public:
	constexpr static const memsize INVALID_MAILBOX = static_cast<memsize>(-1);

	Memory *memory;
	int shift = 0;
	memsize mailboxAddress = INVALID_MAILBOX;
	memobserver_id observerId = 0;
	bool hasMail = false;
	bool isInit = false;

	void init()
	{
		if (mailboxAddress == INVALID_MAILBOX)
			throw std::logic_error("GPU mailbox address not set");
		observe();
		Mailbox mailbox;
		mailbox.readReady(false);
		mailbox.writeReady(true);
		writeMailbox(mailbox);
	}

	Mailbox readMailbox()
	{
		MemoryStreamReader reader(*memory, mailboxAddress);
		Mailbox mailbox = {0};
		mailbox.read = Mail(reader.readUint32());
		reader.skip(sizeof(mailbox.reserved));
		mailbox.poll = reader.readUint32();
		mailbox.sender = reader.readUint32();
		mailbox.status = reader.readUint32();
		mailbox.configuration = reader.readUint32();
		mailbox.write = Mail(reader.readUint32());
		return mailbox;
	}

	void writeMailbox(const Mailbox &mailbox)
	{
		MemoryStreamWriter writer(*memory, mailboxAddress);
		writer.writeUint32(mailbox.read);
		writer.skip(sizeof(mailbox.reserved));
		writer.writeUint32(mailbox.read);
		writer.writeUint32(mailbox.sender);
		writer.writeUint32(mailbox.status);
		writer.writeUint32(mailbox.configuration);
		writer.writeUint32(mailbox.write);
	}

	void observe()
	{
		unobserve();
		observerId = memory->observe(mailboxAddress, sizeof(Mailbox),
			[this](memsize m, Access event){this->pickupMail(m, event);},
			Access::Write | Access::Read);
	}

	void unobserve()
	{
		if (observerId != 0)
		{
			memory->unobserve(observerId);
			observerId = 0;
		}
	}

	void pickupMail(memsize address, Access event)
	{
		std::cerr << "picking up mail " << std::hex << address << std::dec << std::endl;
		// Unatomic and excessive just to write 1 bit.
		// Would cause undefined behavior if GPU and CPU
		// were to run on separate threads.
		//
		// Unknown: how the actual hardware behaves if CPU
		// writes to mailbox when write flag is unready?
		Mailbox mail = readMailbox();
		size_t addressAligned = address & (~static_cast<typeof(address)>(0b11));
		if (event == Access::Write && mail.isWriteReady())
		{
			size_t writeOffset = offsetof(typeof(mail), write);
			size_t writeAddress = mailboxAddress + writeOffset;
			if (addressAligned == writeAddress)
			{
				mail.writeReady(false);
				hasMail = true;
			}
		}
		if (event == Access::Read)
		{
			size_t readOffset = offsetof(typeof(mail), read);
			size_t readAddress = mailboxAddress + readOffset;
			if (addressAligned == readAddress)
			{
				mail.readReady(false);
			}
		}
		writeMailbox(mail);
	}

	Mail readMessage(const Mail &message)
	{
		std::cerr << "picked up a message, HAHA" << message.channel << message.message << std::endl;
		return Mail();
	}
};

DPointered(Gpu)

Gpu::Gpu(Memory &memory)
{
	d->memory = &memory;
}

Gpu::~Gpu()
{
	d->unobserve();
}

void Gpu::cycle()
{
	if (!d->isInit)
	{
		d->init();
		d->isInit = true;
	}
	if (d->hasMail)
	{
		Mailbox mailbox = d->readMailbox();
		Mail response = d->readMessage(mailbox.write);
		d->hasMail = false;
		mailbox.write = response;
		mailbox.writeReady(true);
		mailbox.readReady(true);
		d->writeMailbox(mailbox);
	}
	++d->shift;
}

void Gpu::draw(Canvas &canvas)
{
	canvas.begin();
	canvas.changeSize(640, 480);
	for (int x = 0; x < 640; ++x)
	{
		for (int y = 0; y < 480; ++y)
		{
			canvas.drawPixel(x, y, Color((x + d->shift) % 255, 128, y % 255));
		}
	}
	canvas.drawPixel(0, 0, Color(255, 0, 255));
	canvas.end();
}

void Gpu::setMailboxAddress(memsize address)
{
	if (d->isInit)
		throw std::logic_error("cannot change GPU mailbox address after init");
	d->mailboxAddress = address;
}
