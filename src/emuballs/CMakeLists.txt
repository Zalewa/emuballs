# Copyright 2016 Zalewa <zalewapl@gmail.com>.
#
# This file is part of Emuballs.
#
# Emuballs is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Emuballs is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Emuballs.  If not, see <http://www.gnu.org/licenses/>.
set(SOURCES
	armgpu.cpp
	armmachine.cpp
	armopcode.cpp
	armopcode_dataproc_psr.cpp
	armopcode_impl.cpp
	armregisterset.cpp
	canvas.cpp
	device.cpp
	device_pi.cpp
	memory.cpp
	opdecoder.cpp
	picture.cpp
	registerset.cpp
	regval.cpp
	../strings.cpp
	)

add_library(emuballs_static STATIC ${SOURCES})
add_library(emuballs SHARED ${SOURCES})