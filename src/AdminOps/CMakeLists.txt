if(NOT BS_ARCH STREQUAL "x64")
  return()
endif()

# Qt6 的 imported target 自带 /utf-8，与全局的 /source-charset:utf-8 互斥(cl D8016)；
# 此处剔除全局那一个即可——/utf-8 本就包含“以 UTF-8 读源码”，本目录无需重复指定
get_property(_compile_opts DIRECTORY PROPERTY COMPILE_OPTIONS)
list(REMOVE_ITEM _compile_opts /source-charset:utf-8)
set_property(DIRECTORY PROPERTY COMPILE_OPTIONS "${_compile_opts}")

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