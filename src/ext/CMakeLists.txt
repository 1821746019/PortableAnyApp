set(lib_name ext_common)
bs_add_lib(${lib_name} STATIC "_common/*.ixx")

#set(lib_name extra)

add_library(pch STATIC pch.cpp)
target_precompile_headers(pch PRIVATE pch.cpp)
link_libraries(${lib_name} ext_common Hooker pch)

### 每个文件夹为一个target
# 获取当前目录的所有文件和文件夹的名字
file(GLOB file_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

# 过滤出文件夹
set(dir_list "")
foreach(DIR ${file_list})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DIR})
    #排除前缀为_的
    if(DIR MATCHES "^_")
      continue()
    endif()
    list(APPEND dir_list ${DIR})
  endif()
endforeach()

# 每个文件夹(除前缀为_的)为一个target
foreach(i ${dir_list})

  bs_add_lib(${i} SHARED ${i}/*.ixx ${i}/*.cpp)
  set_target_properties(${i} PROPERTIES FOLDER "ext")
  
  if(${i} MATCHES "Qt5$")
      target_link_libraries(${i} PRIVATE Qt5::Core Qt5::Network Qt5::Widgets Qt5::Gui)
  elseif(${i} MATCHES "Qt6$")
      target_link_libraries(${i} PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Gui)
  endif()
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




set(curr_target  RegCore)
#bs_add_lib(${curr_target} SHARED *.ixx)
target_link_libraries(${curr_target} PRIVATE ntdll  advapi32 #advapi32Forged
kernel32
crypt32
bcrypt
wbemuuid)
target_link_libraries(${curr_target} PRIVATE winreg Hooker)

set(curr_target fs_core)
target_link_libraries(${curr_target} PRIVATE ntdll MyLib Hooker)
