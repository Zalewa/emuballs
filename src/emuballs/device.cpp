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
#include "emuballs/device.hpp"

#include "dptr_impl.hpp"
#include "programmer_impl.hpp"

#include "device_pi.hpp"

using namespace Emuballs;

namespace Emuballs
{

DClass<Device>
{
public:
	std::shared_ptr<Programmer> programmer;
};

DPointeredNoCopy(Device)

}

Device::Device()
{
	d->programmer.reset(new NoProgrammer(*this));
}

Device::~Device() {}

Programmer &Device::programmer()
{
	return *d->programmer;
}

void Device::setProgrammer(std::shared_ptr<Programmer> programmer)
{
	d->programmer = programmer;
}

///////////////////////////////////////////////////////////////////////////

namespace Emuballs
{
DClass<DeviceFactory>
{
public:
	std::string name;
	device_factory factory;
};

DPointered(DeviceFactory)
}

DeviceFactory::DeviceFactory()
{
}

DeviceFactory::DeviceFactory(const std::string &name, device_factory &factory)
{
	d->name = name;
	d->factory = factory;
}

DeviceFactory::DeviceFactory(const DeviceFactory &other)
{
	d = other.d;
}

const DeviceFactory &DeviceFactory::operator=(const DeviceFactory &other)
{
	if (this != &other)
		d = other.d;
	return *this;
}

DeviceFactory::~DeviceFactory()
{
}

DevicePtr DeviceFactory::create() const
{
	if (!isValid())
		return nullptr;
	return d->factory();
}

std::string &DeviceFactory::name() const
{
	return d->name;
}

bool DeviceFactory::isValid() const
{
	return d->factory != nullptr;
}

///////////////////////////////////////////////////////////////////////////

std::list<DeviceFactory> Emuballs::listDevices()
{
	return Pi::listPiDevices();
}
