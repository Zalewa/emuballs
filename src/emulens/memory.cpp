/*
 * Copyright 2017 Zalewa <zalewapl@gmail.com>.
 *
 * This file is part of Emulens.
 *
 * Emulens is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Emulens is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Emulens.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "memory.hpp"

#include "ui_memory.h"

#include <emuballs/memory.hpp>
namespace Emulens
{
class MemoryIo : public QIODevice
{
public:
	MemoryIo(Emuballs::Memory &memory, Emuballs::memsize baseOffset, Emuballs::memsize length)
		: memory(memory), _base(baseOffset), _length(length)
	{
		_pos = 0;
	}

	bool open(OpenMode mode) override
	{
		_pos = 0;
		return QIODevice::open(mode);
	}

	qint64 pos() const override
	{
		return _pos;
	}

	bool reset() override
	{
		_pos = 0;
		return QIODevice::reset();
	}

	qint64 size() const override
	{
		return _length;
	}

	bool isSequential() const override
	{
		return false;
	}

protected:
	qint64 readData(char *data, qint64 maxSize) override
	{
		std::vector<uint8_t> chunk = memory.chunk(_base + _pos, maxSize);
		_pos += chunk.size();
		memcpy(data, chunk.data(), chunk.size());
		return chunk.size();
	}

	qint64 writeData(const char *data, qint64 maxSize) override
	{
		qint64 written = 0;
		std::vector<uint8_t> chunk(
			reinterpret_cast<const uint8_t*>(data),
			reinterpret_cast<const uint8_t*>(data + maxSize));
		written = memory.putChunk(_base + _pos, chunk);
		_pos += written;
		return written;
	}

private:
	Emuballs::Memory &memory;
	qint64 _pos;
	qint64 _base;
	qint64 _length;
};
}

DClass<Emulens::Memory> : public Ui::Memory
{
public:
	std::shared_ptr<Emuballs::Device> device;
	Emuballs::memsize offset = 0;
	QHexDocument *document;

	std::shared_ptr<QIODevice> io()
	{
		auto io = std::shared_ptr<QIODevice>(new Emulens::MemoryIo(
				device->memory(), offset, device->memory().pageSize()));
		io->open(QIODevice::ReadWrite);
		return io;
	}

	std::shared_ptr<QIODevice> io(Emuballs::memsize offset)
	{
		this->offset = offset;
		return io();
	}
};

using namespace Emulens;

DPointeredNoCopy(Memory);

Memory::Memory(std::shared_ptr<Emuballs::Device> device, QWidget *parent)
	: QWidget(parent)
{
	d->setupUi(this);
	d->device = device;
}

void Memory::savePage()
{
	// TODO: Writing at the end of editor induces a crash
	// due to an uncaught exception. Resolve this somehow.
	std::shared_ptr<QIODevice> device = d->io();
	d->document->saveTo(device.get());
}

void Memory::showOffsetFromListItem(QListWidgetItem *item)
{
	Emuballs::memsize offset = item->text().toULongLong(Q_NULLPTR, 16);
	// TODO: Something wicked is going on here: I can't find any place
	// where document is deleted. It should be investigated further.
	std::shared_ptr<QIODevice> device = d->io(offset);
	d->document = QHexDocument::fromDevice(device.get());
	d->document->setBaseAddress(offset);
	connect(d->document, &QHexDocument::documentChanged, this, &Memory::savePage);
	d->hexEdit->setDocument(d->document);
}

void Memory::update()
{
	std::vector<Emuballs::memsize> pages = d->device->memory().allocatedPages();
	d->pagesList->clear();
	for (Emuballs::memsize page : pages)
		d->pagesList->addItem(QString("%1").arg(static_cast<size_t>(page), 8, 16, QChar('0')));
}
