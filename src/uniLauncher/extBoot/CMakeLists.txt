

set(dll_name extBoot)

bs_add_lib(${dll_name} SHARED
	*.cpp *.ixx )

target_link_libraries(${dll_name} PRIVATE versionForged winmmForged)
add_custom_command(TARGET ${dll_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${dll_name}.dll" "${CMAKE_CURRENT_BINARY_DIR}/${dll_name}$<$<STREQUAL:${BS_ARCH},x86>:.${BS_ARCH}>.dll"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/${dll_name}$<$<STREQUAL:${BS_ARCH},x86>:.${BS_ARCH}>.dll"
        "D:/Program/OSEnhance/7-Zip/7-ZipPortable/version.dll"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_BINARY_DIR}/${dll_name}$<$<STREQUAL:${BS_ARCH},x86>:.${BS_ARCH}>.dll"
            "${CMAKE_CURRENT_BINARY_DIR}/../version.dll"
)
