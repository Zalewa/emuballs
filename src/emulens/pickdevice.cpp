/*
 * Copyright 2016 Zalewa <zalewapl@gmail.com>.
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
#include "pickdevice.hpp"

#include "ui_pickdevice.h"
#include <emuballs/device.hpp>
#include <QMessageBox>
#include <QString>

using namespace Emulens;

DClass<PickDevice> : public Ui::PickDevice
{
public:
	std::list<Emuballs::DeviceFactory> factories;
	Emuballs::DeviceFactory pickedFactory;

	void loadDevices()
	{
		factories = Emuballs::listDevices();
		int i = 0;
		for (Emuballs::DeviceFactory &factory : factories)
		{
			auto *item = new QListWidgetItem(QString::fromStdString(factory.name()));
			item->setData(Qt::UserRole, i);
			devicesList->addItem(item);
			++i;
		}
	}

	Emuballs::DeviceFactory selectedFactory() const
	{
		auto *item = devicesList->currentItem();
		if (item == nullptr)
			return Emuballs::DeviceFactory();
		return *(std::next(factories.cbegin(), item->data(Qt::UserRole).toInt()));
	}
};

DPointered(PickDevice)

PickDevice::PickDevice(QWidget *parent)
	: QDialog(parent)
{
	d->setupUi(this);
	d->loadDevices();
	d->devicesList->setFocus();
	d->devicesList->setCurrentRow(0);
}

void PickDevice::accept()
{
	d->pickedFactory = d->selectedFactory();
	if (d->pickedFactory.isValid())
		QDialog::accept();
	else
		QMessageBox::critical(this, tr("Emulens"), tr("No device was selected."));
}

Emuballs::DeviceFactory PickDevice::pickedDeviceFactory() const
{
	return d->pickedFactory;
}
