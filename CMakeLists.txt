# CMakeList.txt : CMake project for PortableMyApp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
include(.cmake/.cmake)
# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")


project (PortableAnyApp)


# vcpkg config

#set(VCPKG_OVERLAY_TRIPLETS "x64-win-static")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
add_compile_definitions($<$<CONFIG:Debug>:BS_DBG>)
add_definitions(-U_DEBUG)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "Debug flags" FORCE)
if(MSVC AND CMAKE_CXX_FLAGS_DEBUG MATCHES "/MDd")
    string(REGEX REPLACE "/MDd" "/MD" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

#add_compile_options("/MD")
# Add source to this project's executable.
add_subdirectory(external)
add_subdirectory(src)
add_subdirectory("test")
#add_subdirectory("cmake")


# TODO: Add tests and install targets if needed.
