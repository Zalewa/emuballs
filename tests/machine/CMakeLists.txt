function(def_machine_module name units)
	def_module(${name} ${units})
	target_link_libraries(${name} machine)
endfunction()

def_machine_module(machine_armflags armflags.cpp)
def_machine_module(machine_armopcode_conditions armopcode_conditions.cpp)
def_machine_module(machine_memory memory.cpp)
def_machine_module(machine_opdecoder opdecoder.cpp)
