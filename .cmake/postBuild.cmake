function(copyToInstallDir targetName baseDir)
     #默认直接使用目标文件名
    set(targetFilePath "${PROJECT_SOURCE_DIR}/install/${BS_ARCH}/${baseDir}/$<TARGET_FILE_NAME:${targetName}>")
    # 共享库（DLL）要为x86的dll添加.x86
    get_target_property(targetType ${targetName} TYPE)
    if(targetType STREQUAL "SHARED_LIBRARY")
        
        # 添加后缀逻辑，基于架构添加后缀
        set(targetFilePath "${PROJECT_SOURCE_DIR}/install/${BS_ARCH}/${baseDir}/$<TARGET_FILE_NAME:${targetName}>$<$<STREQUAL:${BS_ARCH},x86>:.${BS_ARCH}>")
        
        # 针对 DLL 添加 .dll 后缀
        if(WIN32)
            set(targetFilePath "${targetFilePath}.dll")
        endif()
       
    endif()

    # 添加自定义命令，拷贝文件到安装目录
    add_custom_command(TARGET ${targetName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${targetName}>"
            "${targetFilePath}"
    )
endfunction()