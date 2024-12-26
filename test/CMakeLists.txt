
add_executable(dll_test "version_dll_test.cpp")
target_link_libraries(dll_test "version")


set(exe_name "PortableTest")

add_executable(${exe_name} PortableTest.cpp)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../external)


set(exe_name testApp)
add_executable(${exe_name} testApp.cpp)
#target_link_libraries(${exe_name} PRIVATE Boost::spirit)
#target_include_directories(${exe_name} PRIVATE ${Live++_INCLUDE_DIRS})


find_package(boost_spirit REQUIRED CONFIG)
link_libraries( Boost::spirit)
set(exe_name boost.spirit.test)

bs_add_exe(${exe_name} boost.spirit.cpp)



add_dependencies(testApp extBoot loadDllInChildren allocConsole )


set(exe_name tmp)
bs_add_exe(${exe_name} tmp.ixx)
target_precompile_headers(${exe_name} REUSE_FROM winreg)
target_link_libraries(${exe_name} PRIVATE RegCore)

set(exe_name RegParser)

bs_add_exe(${exe_name} RegParser.ixx)