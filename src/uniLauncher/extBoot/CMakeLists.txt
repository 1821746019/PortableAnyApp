

set(dll_name extBoot)

bs_add_lib(${dll_name} SHARED
	*.cpp *.ixx )

target_link_libraries(${dll_name} PRIVATE versionForged winmmForged)
add_custom_command(TARGET ${dll_name} POST_BUILD

    # for install dir
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${dll_name}>
            "${PROJECT_SOURCE_DIR}/install/${BS_ARCH}/${dll_name}$<$<STREQUAL:${BS_ARCH},x86>:.${BS_ARCH}>.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${dll_name}.toml" "${PROJECT_SOURCE_DIR}/install/${BS_ARCH}/${dll_name}.toml"
    

#     COMMAND ${CMAKE_COMMAND} -E copy
#         "${CMAKE_CURRENT_BINARY_DIR}/${dll_name}$<$<STREQUAL:${BS_ARCH},x86>:.${BS_ARCH}>.dll"
#         "D:/Program/Productivity/Utils/PixPin/v1.9.5/App/version.dll"
)
