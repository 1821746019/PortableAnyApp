
add_executable(dll_test "version_dll_test.cpp")
target_link_libraries(dll_test "version")


set(exe_name "PortableTest")

add_executable(${exe_name} PortableTest.cpp)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../external)


set(exe_name testApp)
add_executable(${exe_name} testApp.cpp)
find_package(boost_spirit REQUIRED CONFIG)
target_link_libraries(${exe_name} PRIVATE Boost::spirit)
target_include_directories(${exe_name} PRIVATE ${Live++_INCLUDE_DIRS})
#add_custom_command(${exe_name} POST_BUILD)

