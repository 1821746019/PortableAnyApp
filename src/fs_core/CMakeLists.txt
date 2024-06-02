set(lib_name "fs_core")

bs_add_lib(${lib_name}  "*.cpp" "*.ixx")

target_link_libraries(${lib_name} PRIVATE ntdll MyLib Hooker common)

