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
function(def_emuballs_module name units)
	def_module(${name} ${units})
	target_link_libraries(${name} emuballs_static)
endfunction()

function(def_emuballs_module_shared name units)
	set(target_name ${name}_shared)
	def_module(${target_name} ${units})
	target_link_libraries(${target_name} emuballs)
endfunction()

def_emuballs_module(emuballs_array_queue array_queue.cpp)
def_emuballs_module(emuballs_arm_arithmetic_carry_overflow arm_arithmetic_carry_overflow.cpp)
def_emuballs_module(emuballs_armflags armflags.cpp)
def_emuballs_module(emuballs_armopcode_branch armopcode_branch.cpp)
def_emuballs_module(emuballs_armopcode_block_data_transfer armopcode_block_data_transfer.cpp)
def_emuballs_module(emuballs_armopcode_byte_reverse armopcode_byte_reverse.cpp)
def_emuballs_module(emuballs_armopcode_conditions armopcode_conditions.cpp)
def_emuballs_module(emuballs_armopcode_double_data_transfer armopcode_double_data_transfer.cpp)
def_emuballs_module(emuballs_armopcode_factory armopcode_factory.cpp)
def_emuballs_module(emuballs_armopcode_dataproc_psr armopcode_dataproc_psr.cpp)
def_emuballs_module(emuballs_armopcode_multiply armopcode_multiply.cpp)
def_emuballs_module(emuballs_armopcode_multiply_long armopcode_multiply_long.cpp)
def_emuballs_module(emuballs_armopcode_single_data_transfer armopcode_single_data_transfer.cpp)
def_emuballs_module(emuballs_armopcode_single_data_swap armopcode_single_data_swap.cpp)
def_emuballs_module(emuballs_armregisterset armregisterset.cpp)
def_emuballs_module(emuballs_device_factory device_factory.cpp)
def_emuballs_module_shared(emuballs_device_factory device_factory.cpp)
def_emuballs_module(emuballs_memory memory.cpp)
def_emuballs_module(emuballs_namedregister namedregister.cpp)
def_emuballs_module(emuballs_opdecoder opdecoder.cpp)
def_emuballs_module(emuballs_programs programs.cpp)
def_emuballs_module(emuballs_shift shift.cpp)
