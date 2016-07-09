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
#include "registers.hpp"

#include "ui_registers.h"

#include <emuballs/registerset.hpp>
#include <emuballs/regval.hpp>
#include <strings.hpp>
#include <QDebug>

using namespace Emulens;

DClass<Registers> : public Ui::Registers
{
public:
	std::shared_ptr<Emuballs::Device> device;

	QString regId(const Emuballs::NamedRegister &reg)
	{
		return QString::fromStdString(reg.names().front());
	}
};

DPointeredNoCopy(Registers)

Registers::Registers(std::shared_ptr<Emuballs::Device> device, QWidget *parent)
	: QWidget(parent)
{
	d->setupUi(this);

	d->device = device;

	d->editArray->setInputMask("hhhhhhhh");
	Emuballs::RegisterSet &regs = d->device->registers();
	for (const Emuballs::NamedRegister &reg : regs.registers())
	{
		d->editArray->addEditor(
			QString::fromStdString(Strings::concat(reg.names(), ",")),
			d->regId(reg));
	}
	update();
}

void Registers::setRegisterToHex(const QString &regName, const QString &hexValue)
{
	Emuballs::RegisterSet &regs = d->device->registers();
	bool ok = false;
	auto value = hexValue.toUInt(&ok, 16);
	qDebug() << "Emulens::Registers - setting register" << regName << " to " << hexValue;
	if (ok)
	{
		regs.setReg(regName.toStdString(), value);
		update();
	}
	else
	{
		qDebug() << "Emulens::Registers - wrong hex value" << hexValue;
	}
}

void Registers::update()
{
	Emuballs::RegisterSet &regs = d->device->registers();
	for (const Emuballs::NamedRegister &reg : regs.registers())
	{
		QString hexValue = QString::number(reg.value(), 16);
		hexValue = hexValue.rightJustified(8, '0');
		d->editArray->setValue(d->regId(reg), hexValue);
	}
}
