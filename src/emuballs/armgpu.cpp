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

#include "emuballs/canvas.hpp"
#include "emuballs/color.hpp"

#include "memory.hpp"

#include <cstddef>
#include <functional>
using namespace Emuballs;
using namespace Emuballs::Arm;

namespace Emuballs
{
namespace Arm
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
	uint32_t reserved[3];
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
		return statusBit(StatusBit::ReadReady);
	}

	void readReady(bool ready)
	{
		statusBit(StatusBit::ReadReady, ready);
	}

	bool isWriteReady() const
	{
		return statusBit(StatusBit::WriteReady);
	}

	void writeReady(bool ready)
	{
		statusBit(StatusBit::WriteReady, ready);
	}

private:
	bool statusBit(StatusBit bit) const
	{
		// 0 - enabled, 1 - disabled
		return (status & static_cast<uint32_t>(bit)) == 0;
	}

	void statusBit(StatusBit bit, bool enable)
	{
		// 0 - enabled, 1 - disabled
		if (enable)
		{
			status &= ~static_cast<uint32_t>(bit);
		}
		else
		{
			status |= static_cast<uint32_t>(bit);
		}
	}
};

struct FrameBufferInfo
{
	uint32_t physicalWidth;
	uint32_t physicalHeight;
	uint32_t virtualWidth;
	uint32_t virtualHeight;
	uint32_t pitch; // out
	uint32_t bitDepth;
	uint32_t xOffset;
	uint32_t yOffset;
	uint32_t pointer; // out
	uint32_t size; // out
};

enum class BitDepth : int
{
	HighColor = 16
};
}
}

namespace Emuballs
{

DClass<Gpu>
{
public:
	constexpr static const memsize INVALID_ADDRESS = static_cast<memsize>(-1);

	Memory *memory;
	memsize mailboxAddress = INVALID_ADDRESS;
	memsize frameBufferPointerEnd = INVALID_ADDRESS;
	memobserver_id observerId = 0;
	bool hasMail = false;
	bool isInit = false;
	std::shared_ptr<FrameBufferInfo> frameBufferInfo;

	void init()
	{
		if (mailboxAddress == INVALID_ADDRESS)
			throw std::logic_error("GPU mailbox address not set");
		if (frameBufferPointerEnd == INVALID_ADDRESS)
			throw std::logic_error("Frame Buffer pointer end not set");
		observe();
		Mailbox mailbox;
		mailbox.readReady(false);
		mailbox.writeReady(true);
		writeMailbox(mailbox);
	}

	Mailbox readMailbox()
	{
		MemoryStreamReader reader(*memory, mailboxAddress);
		Mailbox mailbox;
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

	FrameBufferInfo readFrameBufferInfo(memsize address)
	{
		MemoryStreamReader reader(*memory, address);
		FrameBufferInfo fb;
		fb.physicalWidth = reader.readUint32();
		fb.physicalHeight = reader.readUint32();
		fb.virtualWidth = reader.readUint32();
		fb.virtualHeight = reader.readUint32();
		fb.pitch = reader.readUint32();
		fb.bitDepth = reader.readUint32();
		fb.xOffset = reader.readUint32();
		fb.yOffset = reader.readUint32();
		fb.pointer = reader.readUint32();
		fb.size = reader.readUint32();
		return fb;
	}

	void writeFrameBufferInfo(memsize address, const FrameBufferInfo &fb)
	{
		MemoryStreamWriter writer(*memory, address);
		writer.writeUint32(fb.physicalWidth);
		writer.writeUint32(fb.physicalHeight);
		writer.writeUint32(fb.virtualWidth);
		writer.writeUint32(fb.virtualHeight);
		writer.writeUint32(fb.pitch);
		writer.writeUint32(fb.bitDepth);
		writer.writeUint32(fb.xOffset);
		writer.writeUint32(fb.yOffset);
		writer.writeUint32(fb.pointer);
		writer.writeUint32(fb.size);
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
		// Unatomic and excessive just to write 1 bit.
		// Would cause undefined behavior if GPU and CPU
		// were to run on separate threads.
		//
		// Unknown: how the actual hardware behaves if CPU
		// writes to mailbox when write flag is unready?
		Mailbox mail = readMailbox();
		size_t addressAligned = address & (~static_cast<decltype(address)>(0b11));
		if (event == Access::Write && mail.isWriteReady())
		{
			size_t writeOffset = offsetof(decltype(mail), write);
			size_t writeAddress = mailboxAddress + writeOffset;
			if (addressAligned == writeAddress)
			{
				mail.writeReady(false);
				hasMail = true;
			}
		}
		if (event == Access::Read)
		{
			size_t readOffset = offsetof(decltype(mail), read);
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
		// 0x40000000 is a special cache flag, so let's filter
		// it out and all bits above it.
		// https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/screen01.html
		memsize frameBufferAddress = message.message & 0x3fffffff;

		FrameBufferInfo frameBufferInfo = readFrameBufferInfo(frameBufferAddress);
		uint32_t bytesPerPixel = frameBufferInfo.bitDepth / 8;
		if ((frameBufferInfo.bitDepth % 8) != 0)
			bytesPerPixel++;
		frameBufferInfo.pitch = frameBufferInfo.virtualWidth * bytesPerPixel;
		// How does the GPU pick the pointer? How does it know it won't
		// collide with anything? More documentation must be found, but
		// for now let's pick an arbitrary place.
		frameBufferInfo.size = frameBufferInfo.pitch * frameBufferInfo.virtualHeight;
		frameBufferInfo.pointer = frameBufferPointerEnd - frameBufferInfo.size;
		writeFrameBufferInfo(frameBufferAddress, frameBufferInfo);
		this->frameBufferInfo.reset(new FrameBufferInfo(frameBufferInfo));

		Mail response;
		response.channel = message.channel;
		response.message = 0;
		return response;
	}

	void drawRgb16(Canvas &canvas)
	{
		auto &fbInfo = frameBufferInfo;
		std::vector<uint8_t> frameBuffer = memory->chunk(fbInfo->pointer, fbInfo->size);
		size_t pitch = 0;
		for (size_t y = 0; y < fbInfo->virtualHeight; ++y)
		{
			for (size_t x = 0; x < fbInfo->virtualWidth; ++x)
			{
				uint8_t rawcolor[2];
				rawcolor[0] = frameBuffer[pitch];
				rawcolor[1] = frameBuffer[pitch + 1];

				canvas.drawPixel(x, y, Color(
						(((rawcolor[0] & 0b11111000) >> 3) * 255) / 31,
						((((rawcolor[0] & 0b111) << 3) | ((rawcolor[1] & 0b11100000) >> 5)) * 255) / 63,
						(rawcolor[1] & 0b11111) * 255 / 31
					)
				);

				pitch += 2;
			}
		}
	}

};

DPointered(Gpu)
}

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
		mailbox.read = response;
		mailbox.writeReady(true);
		mailbox.readReady(true);
		d->writeMailbox(mailbox);
	}
}

void Gpu::draw(Canvas &canvas)
{
	if (d->frameBufferInfo == nullptr)
		return;

	auto &fbInfo = d->frameBufferInfo;

	canvas.begin();
	canvas.changeSize(fbInfo->virtualWidth, fbInfo->virtualHeight);
	if (fbInfo->bitDepth == static_cast<int>(BitDepth::HighColor))
	{
		d->drawRgb16(canvas);
	}
	canvas.end();
}

void Gpu::setFrameBufferPointerEnd(memsize address)
{
	if (d->isInit)
		throw std::logic_error("cannot change GPU frame buffer pointer end after init");
	d->frameBufferPointerEnd = address;
}

void Gpu::setMailboxAddress(memsize address)
{
	if (d->isInit)
		throw std::logic_error("cannot change GPU mailbox address after init");
	d->mailboxAddress = address;
}
