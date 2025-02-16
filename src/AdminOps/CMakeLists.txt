# for qt
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)
link_libraries( Qt6::Core Qt6::Network  Qt6::Widgets  Qt6::Gui)
#add_definitions(-D_UTF8)

add_library(qt_pch STATIC pch.cpp)
target_precompile_headers(qt_pch PRIVATE pch.cpp)


set(lib_name AdminOps)
bs_add_lib(${lib_name} SHARED *.ixx)
target_precompile_headers(${lib_name} REUSE_FROM qt_pch)


set(exe_name AdminOpsService)
bs_add_exe(${exe_name} WIN32 service/*.ixx )
target_precompile_headers(${exe_name} REUSE_FROM qt_pch)

requireAdminForExe(${exe_name})
#target_link_libraries(${exe_name} PRIVATE allocConsole)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	target_link_options(${exe_name} PRIVATE "/subsystem:console")
endif()