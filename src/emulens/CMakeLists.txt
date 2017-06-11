set(NAME emulens)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

set(SOURCES
	hexedit/qhexedit.cpp
	hexedit/qhexeditprivate.cpp
	hexedit/document/gapbuffer.cpp
	hexedit/document/qhexcursor.cpp
	hexedit/document/qhexdocument.cpp
	hexedit/document/qhextheme.cpp
	hexedit/document/commands/hexcommand.cpp
	hexedit/document/commands/insertcommand.cpp
	hexedit/document/commands/removecommand.cpp
	hexedit/document/commands/replacecommand.cpp
	hexedit/document/metadata/qhexmetadata.cpp
	hexedit/document/metadata/qhexmetadataitem.cpp
	hexedit/paint/qhexmetrics.cpp
	hexedit/paint/qhexpainter.cpp
	cycler.cpp
	device.cpp
	editarray.cpp
	main.cpp
	mainwindow.cpp
	memory.cpp
	registers.cpp
	trackablemdiwindow.cpp
	pickdevice.cpp
	version.cpp
	)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(${NAME} ${SOURCES})
target_link_libraries(${NAME} emuballs Qt5::Widgets)

if (WIN32)
	# Disable console window in Windows release builds.
	if (MSVC)
		set_target_properties(emulens PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
		set_target_properties(emulens PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
		set_target_properties(emulens PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
	else ()
		set_target_properties(emulens PROPERTIES LINK_FLAGS_DEBUG "-Wl,-subsystem,console")
		set_target_properties(emulens PROPERTIES LINK_FLAGS_RELEASE "-Wl,-subsystem,windows")
		set_target_properties(emulens PROPERTIES LINK_FLAGS_MINSIZEREL "-Wl,-subsystem,windows")
	endif ()
endif ()
