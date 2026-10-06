
BS_ADD_ALL_SUBDIR_IF_EXIST()

set(lib_name winreg)
#bs_add_lib(${lib_name} STATIC winreg.cpp)

find_path(WINREG_INCLUDE_DIRS "winreg/WinReg.hpp")
#target_include_directories(${lib_name} PRIVATE ${WINREG_INCLUDE_DIRS})
bs_add_lib(${lib_name} STATIC void.cpp)

target_precompile_headers(${lib_name} PRIVATE "${WINREG_INCLUDE_DIRS}/winreg/WinReg.hpp")