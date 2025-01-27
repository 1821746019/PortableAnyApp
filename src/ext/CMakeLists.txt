
set(lib_name ext_common)
bs_add_lib(${lib_name} OBJECT *.ixx)

add_library(pch STATIC pch.cpp)
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
    # ext 默认都连接versionForge、winmmForge库以供单独使用
    target_link_libraries(${e} PRIVATE versionForged winmmForged)
    # generate toml file for each ext if not exist
    file(TOUCH ${CMAKE_CURRENT_SOURCE_DIR}/${e}/${e}.toml)
    add_custom_command(TARGET ${e} POST_BUILD
        # copy dll to install folder

        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${e}>
        ${PROJECT_SOURCE_DIR}/install/${BS_ARCH}/Ext/${e}$<$<STREQUAL:${BS_ARCH},x86>:.${BS_ARCH}>.dll

        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${e}/${e}.toml ${PROJECT_SOURCE_DIR}/install/${BS_ARCH}/Ext/${e}.toml
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

set(curr_target  RegCore)
#bs_add_lib(${curr_target} SHARED *.ixx)
target_link_libraries(${curr_target} PRIVATE ntdll  advapi32 #advapi32Forged
kernel32
crypt32
bcrypt
wbemuuid)
target_link_libraries(${curr_target} PRIVATE winreg Hooker)

#set(curr_target UniqueWindowGroup)
