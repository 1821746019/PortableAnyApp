
set(lib_name common)
bs_add_lib(${lib_name} "_common/*.ixx" "_common/*.cpp")



find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
target_link_libraries(${lib_name} PkgConfig::tomlplusplus)
#include_directories("external")
get_target_property(link_libraries ${lib_name} LINK_LIBRARIES)
message(WARNING "${lib_name} linked with ${link_libraries}")
target_link_libraries(${lib_name} 
#tomlplusplus::tomlplusplus 
MyLib)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
#add_subdirectory(uniLauncher)
bs_add_all_subdir_if_exist()
