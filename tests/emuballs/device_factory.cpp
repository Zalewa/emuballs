#define BOOST_TEST_MODULE device_factory
#include <boost/test/unit_test.hpp>
#include "emuballs/device.hpp"

#include <string>

BOOST_AUTO_TEST_CASE(device_factory_all_valid)
{
	std::list<Emuballs::DeviceFactory> devices = Emuballs::listDevices();
	for (auto &factory : devices)
	{
		std::string msg =  u8"Factory '" + factory.name() + u8"' is not valid.";
		BOOST_CHECK_MESSAGE(factory.isValid(), msg.data());
	}
}
