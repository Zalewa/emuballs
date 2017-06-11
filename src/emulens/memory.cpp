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
class DataStorageMemory : public HexView::DataStorage
{
public:
	DataStorageMemory(Emuballs::Memory &memory)
		: memory(memory)
	{
	}

	virtual QByteArray getData(std::size_t position, std::size_t length)
	{
		std::vector<uint8_t> chunk = memory.chunk(position, length);
		if (position + length > memory.size())
			length = memory.size() - position;
		return QByteArray(reinterpret_cast<const char*>(chunk.data()), chunk.size());
	}

	virtual std::size_t size()
	{
		return memory.size();
	}

private:
	Emuballs::Memory &memory;
};
}

DClass<Emulens::Memory> : public Ui::Memory
{
public:
	std::shared_ptr<Emuballs::Device> device;
	std::unique_ptr<Emulens::DataStorageMemory> memoryOverlay;
};

using namespace Emulens;

DPointeredNoCopy(Memory);

Memory::Memory(std::shared_ptr<Emuballs::Device> device, QWidget *parent)
	: QWidget(parent)
{
	d->setupUi(this);
	d->device = device;
	d->memoryOverlay.reset(new DataStorageMemory(device->memory()));
	d->hexView->setData(d->memoryOverlay.get());
	d->hexView->showFromOffset(0);
}

void Memory::showOffsetFromListItem(QListWidgetItem *item)
{
	Emuballs::memsize offset = item->text().toULongLong();
	d->hexView->showFromOffset(offset);
}

void Memory::update()
{
	std::vector<Emuballs::memsize> pages = d->device->memory().allocatedPages();
	d->pagesList->clear();
	for (Emuballs::memsize page : pages)
		d->pagesList->addItem(QString("%1").arg(static_cast<size_t>(page), 8, 16, QChar('0')));
}
