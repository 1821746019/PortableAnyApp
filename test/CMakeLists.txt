add_executable(dll_test "version_dll_test.cpp")
target_link_libraries(dll_test "version")


set(exe_name "testApp")

add_executable(${exe_name} testApp.cpp)
target_link_libraries(${exe_name}  "version.lib" ntdll common)
#add_custom_command(${exe_name} POST_BUILD)