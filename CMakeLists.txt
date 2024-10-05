# CMakeList.txt : CMake project for PortableMyApp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.28)


project (PortableAnyApp)
#需在project后include，否则会有一些变量未初始化，e.CMAKE_SIZEOF_VOID_P
include(.cmake/.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)


add_compile_definitions($<$<CONFIG:Debug>:BS_DBG>)
#if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
if(MSVC)
	#Do not use MDd runtime libaray
	#add_definitions(-U_DEBUG)
	#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
	#just debug my code
	add_compile_options(/JMC)
	# enable ASan. this is mutually exclusive with hot reload
	#add_compile_options(-fsanitize=address $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fno-omit-frame-pointer>)
	# enable hot reload 
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
	add_link_options(/FUNCTIONPADMIN)#/FUNCTIONPADMIN means preparing a exe image for hotpatching
endif()


# vcpkg config

#set(VCPKG_OVERLAY_TRIPLETS "x64-win-static")

# Add source to this project's executable.
add_subdirectory(external)
link_libraries(std)





add_subdirectory(src)
add_subdirectory("test")

#add_subdirectory("cmake")


# TODO: Add tests and install targets if needed.
