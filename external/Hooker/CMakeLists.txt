set(lib_name "Hooker")
bs_add_lib( ${lib_name}  "*.ixx" "*.cpp")

# introduce detours lib

target_include_directories(${lib_name} PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(${lib_name} PRIVATE ${DETOURS_LIBRARY})

target_link_libraries(${lib_name} PRIVATE "version.lib" "winhttp")
