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
	Mail read;
	/** Receive without retrieving. */
	uint32_t poll;
	/** Sender information. */
	uint32_t sender;
	uint32_t status;
	uint32_t configuration;
	/** Sending mail. */
	Mail write;

	void readReady(bool ready)
	{
		status &= readyBit(StatusBit::ReadReady, ready);
	}

	void writeReady(bool ready)
	{
		status &= readyBit(StatusBit::WriteReady, ready);
	}

private:
	uint32_t readyBit(StatusBit bit, bool ready)
	{
		return ready ? ~static_cast<uint32_t>(bit) : static_cast<uint32_t>(bit);
	}
};
}

DClass<Gpu>
{
public:
	Memory *memory;
	int shift = 0;
	memsize mailboxAddress;

	Mailbox readMailbox()
	{
		MemoryStreamReader reader(*memory, mailboxAddress);
		Mailbox mailbox = {0};
		mailbox.read = Mail(reader.readUint32());
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
		writer.writeUint32(mailbox.poll);
		writer.writeUint32(mailbox.sender);
		writer.writeUint32(mailbox.status);
		writer.writeUint32(mailbox.configuration);
		writer.writeUint32(mailbox.write);
	}
};

DPointered(Gpu)

Gpu::Gpu(Memory &memory)
{
	d->memory = &memory;
	d->mailboxAddress = 0;
}

void Gpu::cycle()
{
	// Mailbox mailbox = d->readMailbox();
	// mailbox.writeReady(false);
	// mailbox.writeReady(true);
	// d->writeMailbox(mailbox);
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
	d->mailboxAddress = address;
}
