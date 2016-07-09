set(NAME emulens)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

set(SOURCES
	device.cpp
	main.cpp
	mainwindow.cpp
	registers.cpp
	pickdevice.cpp
	version.cpp
	)

set(UI
	pickdevice.ui
	)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(${NAME} ${SOURCES})
target_link_libraries(${NAME} emuballs Qt5::Widgets)
