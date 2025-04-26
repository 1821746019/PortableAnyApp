# add deps
find_package(spdlog CONFIG REQUIRED)
link_libraries(spdlog::spdlog)
find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
add_compile_definitions("-DTOML_HEADER_ONLY=0" "-DTOML_INTELLISENSE=0")
link_libraries(PkgConfig::tomlplusplus)


set(lib_name common)
bs_add_lib(${lib_name} STATIC "_common/*.ixx" )
target_link_libraries(${lib_name} PRIVATE MyLib)

link_libraries(${lib_name})
bs_add_all_subdir_if_exist()

#get_dir_targetList(targetList ${CMAKE_CURRENT_LIST_DIR}/**)

# foreach(target_name ${targetList})
# find_package(spdlog CONFIG REQUIRED)
# target_link_libraries(${target_name} PRIVATE spdlog::spdlog)
# endforeach()