cmake_minimum_required (VERSION 3.8)

if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project ("ICV")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(ConnectionsCollector_SpecificSource ConnectionsCollector_Linux.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(ConnectionsCollector_SpecificSource ConnectionsCollector_Windows.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(ConnectionsCollector_SpecificSource ConnectionsCollector_MacOS.cpp)
else()
    message(FATAL_ERROR "Unknown OS")
endif()

set(SRC
    Application.hpp
    Application.cpp
    ICV.cpp
	ConnectionsCollector.hpp
	ConnectionsCollector.cpp
    ${ConnectionsCollector_SpecificSource}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Build ${PROJECT_NAME} for LINUX")
    add_executable(${PROJECT_NAME} ${SRC})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	message("Build ${PROJECT_NAME} for WINDOWS")
	add_executable(${PROJECT_NAME} ${SRC})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(WARNING "Currently unimplemented!")
    message("Build ${PROJECT_NAME} for APPLE")
    add_executable(${PROJECT_NAME} ${SRC})
else()
    message(WARNING "Build ${PROJECT_NAME} for unknown OS")
    add_executable(${PROJECT_NAME} ${SRC})
endif()
