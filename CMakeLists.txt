# CMakeList.txt : CMake project for PortableMyApp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.30)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")

project(PortableAnyApp)

#需在project后include，否则会有一些变量未初始化，e.CMAKE_SIZEOF_VOID_P
include(.cmake/.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_MODULE_STD ON)

#解决Qt app定义了_UTF8而std模块没有导致有：Possible incompatible environment while importing module 'std': _UTF8 is defined in current command line and not in module command line
add_compile_options(-D_UTF8)#定义_UTF8宏?
#add_compile_options(/utf-8)
add_compile_definitions($<$<CONFIG:Debug>:BS_DBG>)
#if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
if(MSVC)
  # 补齐 CMake File-API 缺失的 IFC 检索路径，供 VS IntelliSense (EDG) 索引
  add_compile_options(/ifcSearchDir "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/__cmake_cxx23.dir")
 
  # 多线程编译
  add_compile_options(/MP)
  # code elimination for release build
  if(CMAKE_BUILD_TYPE MATCHES "^Rel")
    # pure unused code: /OPT:REF /GL /LTCG ; code fold:  /OPT:ICF
      add_link_options(/OPT:REF /OPT:ICF /GL /LTCG)
  endif()
  #Do not use MDd runtime libaray
  #add_definitions(-U_DEBUG)
  #set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
  #just debug my code
  add_compile_options(/JMC)

  # enable ASan. this is mutually exclusive with hot reload
  #add_compile_options(-fsanitize=address $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fno-omit-frame-pointer>)
  # enable hot reload 
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
  add_link_options(/FUNCTIONPADMIN) #/FUNCTIONPADMIN means preparing a exe image for hotpatching
endif()


# vcpkg config

#set(VCPKG_OVERLAY_TRIPLETS "x64-win-static")
# pkg dep
#spdlog
#add_definitions(-DSPDLOG_USE_STD_FORMAT=ON) #使用std::format而不是依赖fmt库

# 使用CMake的作用域来自动恢复变量
#set(_OLD_CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}")
#set(CMAKE_PREFIX_PATH "D:/environment/cpp/Qt/Qt6/_cur" CACHE STRING "首选路径前缀" FORCE)
function(_findQt)
  find_package(Qt6 COMPONENTS Core Network Widgets Gui REQUIRED)
endfunction()
#execInSpecialPath("D:/environment/cpp/Qt/Qt6/_cur" "replace" _findQt)

#set(CMAKE_PREFIX_PATH "${_OLD_CMAKE_PREFIX_PATH}" CACHE STRING "首选路径前缀" FORCE)
#unset(_OLD_CMAKE_PREFIX_PATH)
if(BS_ARCH STREQUAL "x64")
  find_package(Qt6 
      COMPONENTS Core Network Widgets Gui 
      REQUIRED
      PATHS "D:/environment/cpp/Qt/Qt6/_cur"
      NO_DEFAULT_PATH
  )

  find_package(Qt5 
      COMPONENTS Core Network Widgets Gui 
      REQUIRED
  )
endif()

#detours
find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

# Add source to this project's executable.
add_subdirectory(external)




add_subdirectory(src)
add_subdirectory("test")
add_subdirectory("practice")
#add_subdirectory("cmake")


# TODO: Add tests and install targets if needed.
