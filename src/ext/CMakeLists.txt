
set(lib_name ext_common)
bs_add_lib(${lib_name} OBJECT *.ixx)

add_library(pch STATIC  pch.cpp)
target_precompile_headers(pch PRIVATE pch.cpp)
link_libraries(${lib_name} Hooker pch)

### 每个文件夹为一个target
# 获取当前目录的所有文件和文件夹的名字
file(GLOB file_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

# 过滤出文件夹
set(dir_list "")
foreach(DIR ${file_list})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DIR})
        list(APPEND dir_list ${DIR})
    endif()
endforeach()


foreach(i ${dir_list})
    bs_add_lib(${i} SHARED ${i}/*.ixx ${i}/*.cpp)
    set_target_properties(${i} PROPERTIES FOLDER "ext")
endforeach()

# 针对每个target配置

get_dir_targetList(all_targets ${CMAKE_CURRENT_SOURCE_DIR}/)
# 为ext组定义一个逻辑目标
add_custom_target(build_ext ALL)
foreach(e ${all_targets})
    get_target_property(target_type ${e} TYPE)
    if(NOT target_type STREQUAL "SHARED_LIBRARY")
        continue()
    endif()
    target_precompile_headers(${e} REUSE_FROM pch)
    add_dependencies(build_ext ${e})
    add_custom_command(TARGET ${e} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${e}>
        ${PROJECT_SOURCE_DIR}/install/${BS_ARCH}/Ext/${e}$<$<STREQUAL:${BS_ARCH},x86>:.${BS_ARCH}>.dll
        COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_SOURCE_DIR}/install/${BS_ARCH}/Ext/${e}.toml
    )
endforeach()


add_custom_command(TARGET allocConsole POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/allocConsole.dll
    "D:/Program Files (x86)/VMapp/Genymotion/Ext")



set(curr_target HijackDnsQuery)
add_custom_command(TARGET ${curr_target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${curr_target}>"
    "D:/Program/Productivity/Utils/PixPin/v1.9.5/App/version.dll"
)
