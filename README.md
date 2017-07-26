Emuballs / Emulens
==================

Emuballs / Emulens is a Rasberry Pi emulator software programmed
to run "Baking Pi" lessons which are available at this URL:

https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/

Emuballs / Emulens is still in a very early stage of development
and only few features are available. See [Current State](#current-state) below.


Compilation - Linux
===================

This works on Ubuntu 16.04:

```
  sudo apt update
  sudo apt install build-essential cmake libboost-test-dev qt5-default
  cd path/to/repo
  cmake -DCMAKE_BUILD_TYPE=Release .
  make -j `nproc`
```

`emulens` executable will be built in `./bin/emulens` path.

`emuballs` library (both shared and static) will be built in `./lib` path.


Running Unit Tests - Linux
--------------------------

After building, `cd ./tests` and execute `ctest` command.


Compilation - Linux - Docker
----------------------------

```
  cd path/to/repo
  docker build -t emuballs .
  docker run -it emuballs --name emuballs
  cmake -DCMAKE_BUILD_TYPE=Release .
  make -j `nproc`
```


Compilation - Windows
=====================

32-bit builds can be built with **MinGW-w64 i686-dwarf** compiler.

1. Download and install Qt5 open-source from the official home page.
   Pay heed to install Qt5 for the MinGW compiler.

2. Download and compile Boost in accordance to Boost instructions.
   Just building the unit-testing module is sufficient.

3. Download and install CMake. At the moment when this instruction
   was written, the newest CMake version was 3.9.0.

4. Use cmake-gui to generate the project. Pick appropriate build path,
   which should be a non-existent directory. Then for the source path
   show the root of this repository. Then press `Configure`. When prompted,
   use "MinGW Makefiles" as compiler.

5. Initial configuration will probably end in an error but that's expected.

5. Set CMAKE_BUILD_TYPE to Debug or Release, depending on your needs.

6. If Qt5 was not found automatically, set Qt5Widgets_DIR to `lib/cmake/Qt5Widgets`
   directory. For example:

   `P:/Qt/Qt5.7.1-mingw64/5.7/mingw53_32/lib/cmake/Qt5Widgets`

   Then press `Configure` again.

7. If Boost was not found automatically, configure:

   `Boost_INCLUDE_DIR` - Boost root dir.  
   `Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG` - `libboost_unit_test_framework-mgwXX-mt-X_XX.dll`  
   `Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE` - `libboost_unit_test_framework-mgwXX-mt-X_XX.dll`  

   `_RELEASE` and `_DEBUG` get the same .dll

   Then press `Configure` again.

8. CMake should now configure the project completely. Press `Generate`.

9. Run `cmd.exe`

10. `cd` to the build directory.

11. Execute: `cmake --build . -- -j16` - alter the number in `-j16`
    to specify how many parallel compilation jobs you want to run.

`emulens.exe` and `emuballs.dll` will be built in `./bin` subdirectory.
The static Emuballs library and the import library will be built in `./lib` subdirectory.

Depending on your `%PATH%` configuration, it may be necessary to copy
MinGW's, Qt's and Boost's .dlls to run Emulens or the unit tests.

Unit tests are run in the same way as on Linux.


Current State
=============

Emuballs / Emulens is in a very early alpha stage.
It can run Baking Pi lessons OK01 - OK05 and Screen01 - Screen02, Screen04.

Input lessons will not work due to missing USB emulation.

For more info on planned development see the [To Do](#to-do) section.


Emuballs
========

Emuballs is a C++14 library that runs the emulation. It is the brain.

Emuballs implements ARM opcodes, ARM CPU (strongly faked), Raspberry Pi's GPU
(in a most primitive fashion) and RAM. Its API enables front-end applications
to run compiled ARM programs.

Emuballs library has no dependencies, apart from Boost Test which
is necessary to compile and run the unit tests.


Emulens
=======

Emulens is a Qt5 application that serves as a window to Emuballs.

Emulens allows to pick Raspberry Pi device, load a program (the "kernel.img"),
and run it in continuous fashion or by stepping through each opcode.

Emulens provides a view on:

* ARM CPU registers
* RAM
* Video output


To Do
=====

* Performance improvements.
* CPSR flags view.
* GPIO view.
* On-board LEDs view.
* Opcode decompilation to ARM assembly.
* View on currently executed code (ie. code listing).
* Debugging features - break on address execution, break on memory read/write.
* That unit testing should be either made optional or independent of Boost.

To Do - Far Fetched
-------------------

These have minor chances of implementation:

* USB emulation.
* Audio emulation.
* Network (ETH) emulation.
* Special GPIO functionality emulation: UART, SPI, PWM, etc.


Source Materials
================

* [ARM7-TDMI - ARM Instruction Set](http://bear.ces.cwru.edu/eecs_382/ARM7-TDMI-manual-pt2.pdf)
* [ARM Architecture Reference Manual](https://www.scss.tcd.ie/~waldroj/3d1/arm_arm.pdf)
* [BCM2835 ARM Peripherals](https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf)
* [ARM Compiler toolchain Assembler Reference](http://infocenter.arm.com/help/topic/com.arm.doc.dui0489f/DUI0489F_arm_assembler_reference.pdf)


Why Emuballs?
=============

Because we need more software with stupid names.
Also, "Emuballs" is a name that is 37.5% relevant
to what this software does.