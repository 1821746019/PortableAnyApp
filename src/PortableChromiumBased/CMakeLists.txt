set(dll_name PortableChromiumBased)
set(exe_name "test")
set(out_name "version")
file(GLOB src_list "*.cpp" "*.ixx" )

bs_add_lib(${dll_name} SHARED ${src_list} )
set_target_properties(${dll_name} PROPERTIES OUTPUT_NAME ${out_name})

target_link_libraries(${dll_name} PRIVATE versionForged Hooker MyLib)


add_custom_command(TARGET ${dll_name} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy
${CMAKE_CURRENT_BINARY_DIR}/${out_name}.dll
"D:/Program/Productivity/Xmind/XMind2024-v24.01.14361-x64-Repack/app"
)

#add_executable(${exe_name} dll.cpp)
#target_link_libraries(${exe_name} PRIVATE versionForged Hooker MyLib)
