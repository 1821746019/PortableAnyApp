
project(fs_core_driver
)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES TRUE)
set(WDK_PATH "C:/Program Files (x86)/Windows Kits/10")


# 根据系统处理器类型设置编译选项
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
    add_definitions(-D_AMD64_)
    add_definitions(-D_X64_)
    set(ARCH_DIR "x64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64")
    add_definitions(-D_ARM64_)
    set(ARCH_DIR "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM")
    add_definitions(-D_ARM_)
    set(ARCH_DIR "arm")
else()
    message(FATAL_ERROR "Unsupported architecture")
endif()

bs_add_exe(${PROJECT_NAME} #SHARED
	*.h *.cpp *.ixx)
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".sys")

set(SDK_VERSION $ENV{UCRTVersion})

set(WDK_INCLUDE_DIR "${WDK_PATH}/Include/${SDK_VERSION}/km")
message(WARNING "WDK_INCLUDE_DIR is ${WDK_INCLUDE_DIR}")
include_directories(${WDK_INCLUDE_DIR})
set(WDK_LINK_DIR "${WDK_PATH}/Lib/${SDK_VERSION}/km/$ENV{VSCMD_ARG_TGT_ARCH}")
message(WARNING "WDK_LINK_DIR is ${WDK_LINK_DIR}")
link_directories(${WDK_LINK_DIR})

target_link_libraries(${PROJECT_NAME} 
"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/km/x64/ntoskrnl.lib"
"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/km/x64/fltMgr.lib"
)