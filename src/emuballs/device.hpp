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

#include "dptr.hpp"
#include "export.h"
#include <functional>
#include <memory>
#include <list>
#include <string>

namespace Emuballs
{

class Canvas;
class Memory;
class Programmer;
class RegisterSet;

class EMUBALLS_API Device
{
public:
	Device();
	Device(const Device &other) = delete;
	Device &operator=(const Device &other) = delete;
	virtual ~Device();

	virtual void cycle() = 0;
	virtual void draw(Canvas &canvas) = 0;
	virtual Memory &memory() = 0;
	virtual void reset() = 0;
	virtual RegisterSet &registers() = 0;

	Programmer &programmer();

protected:
	void setProgrammer(std::shared_ptr<Programmer> programmer);

private:
	DPtr<Device> d;
};

typedef std::unique_ptr<Device> DevicePtr;
typedef std::function<DevicePtr()> device_factory;

class EMUBALLS_API DeviceFactory
{
public:
	/**
	 * @brief Creates invalid factory.
	 */
	DeviceFactory() = default;
	/**
	 * @brief Creates factory with human-readable name and factory function.
	 */
	DeviceFactory(const std::wstring &name, device_factory &factory);
	virtual ~DeviceFactory();

	/**
	 * @brief Creates a Device with predefined characteristics
	 *        appropriate to that device.
	 *
	 * If factory is not valid then nullptr is returned.
	 */
	DevicePtr create() const;
	/**
	 * @brief Human-readable name of the created Device.
	 */
	std::wstring &name() const;
	/**
	 * @brief Valid factory is a factory that will create() a Device.
	 */
	bool isValid() const;

private:
	DPtr<DeviceFactory> d;
};

std::list<DeviceFactory> listDevices();

}
