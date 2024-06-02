# Windows库
link_libraries(ntdll)
if(bit EQUAL 64)
    add_definitions(-D__x86_64__)
endif()
set(lib_name fs)
bs_add_lib(${lib_name} SHARED
    windows/common/*.c
    windows/common/ntmin/*.c
    shared/*.c
    shared/*.cpp
    #shared/*.h
    windows/filesystem/fs_utils.c
    windows/filesystem/pdxfs.cpp
    
)
set_target_properties(${lib_name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
target_compile_features(${lib_name} PUBLIC cxx_std_20)
#set_target_properties(${lib_name} PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_LIST_DIR}/shared/fs_redirect.def") 
#target_compile_definitions(${lib_name} PRIVATE BYPASS_ENABLED)

set(lib_name reg )
bs_add_lib(${lib_name} SHARED
    windows/common/*.c
    shared/strutils.c
    shared/dbg.c
    windows/common/iniparser/*.c
    windows/common/vector/*.c
    windows/common/ntmin/ntmin.c
    windows/registry/*.c)

set(lib_name proc )
bs_add_lib(${lib_name} SHARED
    windows/common/ntmin/*.c
    windows/common/*.c
    windows/process/pdxproc.c)


# 其他Windows库...
    
bs_add_lib(env SHARED
    windows/common/*.c
    shared/dbg.c
    shared/strutils.c
    windows/common/iniparser/*.c
    windows/environment/*.c)




bs_add_exe(test_pdxfs
    windows/common/*.c
    windows/common/ntmin/*.c
    shared/*.c
    windows/filesystem/fs_utils.c
    #windows/filesystem/test_filesystem.c)
)
#install(TARGET fs E)


#add_custom_target(create_release_windows  COMMAND python ${PROJECT_SOURCE_DIR}/tools/rlsgen.py batteryshark pdx pdx_windows pdxfs32.dll pdxfs64.dll pdxenv32.dll pdxenv64.dll pdxreg32.dll pdxreg64.dll pdxproc32.dll pdxproc64.dll)


# 添加构建目标
#add_custom_target(build_binaries_windows DEPENDS pdxproc64 pdxproc32 pdxreg64 pdxreg32 pdxenv64 pdxenv32 pdxfs64 pdxfs32)


file(GLOB header_list windows/filesystem/*.h windows/process/*.h windows/registry/*.h)
file(COPY ${header_list} DESTINATION "${PROJECT_SOURCE_DIR}/include")
#bs_get_targets(targets ${CMAKE_CURRENT_LIST_DIR})
#message(WARNING "targets is ${targets}")

#get_project_targets(${PROJECT_SOURCE_DIR})
