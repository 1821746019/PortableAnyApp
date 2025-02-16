set(dll_name PortableChromiumBased)
set(exe_name "test")
#set(out_name "winmm")
file(GLOB src_list "*.cpp" "*.ixx" )

bs_add_lib(${dll_name} SHARED ${src_list} )
#set_target_properties(${dll_name} PROPERTIES OUTPUT_NAME ${out_name})

target_link_libraries(${dll_name} PRIVATE versionForged winmmForged Hooker MyLib)




#add_executable(${exe_name} dll.cpp)
#target_link_libraries(${exe_name} PRIVATE versionForged Hooker MyLib)
