set(lib_name common)
bs_add_lib(${lib_name} "*.ixx" "*.cpp")
find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
target_link_libraries(${lib_name} PkgConfig::tomlplusplus)
get_target_property(link_libraries ${lib_name} LINK_LIBRARIES)
message(WARNING "${lib_name} linked with ${link_libraries}")
target_link_libraries(${lib_name} 
#tomlplusplus::tomlplusplus 
MyLib)