
set(lib_name std)

string(REGEX REPLACE "\/bin\/Hostx(64|86)\/x(64|86)\/cl\.exe" "" MSVC_ROOT ${CMAKE_CXX_COMPILER})
# 在配置阶段复制 std.ixx
configure_file(
    "${MSVC_ROOT}/modules/std.ixx" # 源文件路径
    "${CMAKE_CURRENT_SOURCE_DIR}/std.ixx" # 目标文件路径
    COPYONLY # 仅复制，不进行变量替换
)

# 在配置阶段复制 std.compat.ixx
configure_file(
    "${MSVC_ROOT}/modules/std.compat.ixx" # 源文件路径
    "${CMAKE_CURRENT_SOURCE_DIR}/std.compat.ixx" # 目标文件路径
    COPYONLY # 仅复制，不进行变量替换
)

bs_add_lib(${lib_name} STATIC *.ixx)
target_compile_features(${lib_name} PUBLIC cxx_std_23)

# hacky :/




#就算无project了，不把std.ixx和std.compat.ixx复制过来intellisense还是会有问题
# set(lib_name std)

# add_library(${lib_name} STATIC)
# target_compile_features(${lib_name} PUBLIC cxx_std_23)

# # hacky :/
# string(REGEX REPLACE "\/bin\/Hostx(64|86)\/x(64|86)\/cl\.exe" "" MSVC_ROOT ${CMAKE_CXX_COMPILER})

# # std.ixx
# configure_file("${MSVC_ROOT}/modules/std.ixx" "${PROJECT_BINARY_DIR}/std.ixx" COPYONLY)
# string(JOIN " " STD_REF_PARAM /reference "${PROJECT_BINARY_DIR}/CMakeFiles/${lib_name}.dir/std.ifc")
# set(STD_REFERENCE ${STD_REF_PARAM} CACHE INTERNAL "std.ifc compiler parameter")

# # std.compat.ixx
# configure_file("${MSVC_ROOT}/modules/std.compat.ixx" "${PROJECT_BINARY_DIR}/std.compat.ixx" COPYONLY)
# set(STD_COMPAT_REFERENCE /reference "${PROJECT_BINARY_DIR}/CMakeFiles/${lib_name}.dir/std.compat.ifc" CACHE INTERNAL "std.compat.ifc compiler parameter")

# target_sources(${lib_name}
#     PUBLIC
#     FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS ${PROJECT_BINARY_DIR}
#     FILES ${PROJECT_BINARY_DIR}/std.ixx ${PROJECT_BINARY_DIR}/std.compat.ixx
# )

# target_include_directories(${lib_name} PUBLIC ${PROJECT_SOURCE_DIR})



