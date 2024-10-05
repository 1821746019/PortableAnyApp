function(get_dir_targetList _targetList dirPath )
    set(target_list)
    #判断是否end with **来确定匹配模式
    string(REGEX MATCH "\\*\\*$" MR ${dirPath})
    if(MR)
    #remove the ending **
        string(LENGTH ${dirPath} dirPath_len)
        math(EXPR sub_len "${dirPath_len}-2")
        string(SUBSTRING "${dirPath}" 0 ${sub_len} dirPath)
        file(GLOB_RECURSE l ${dirPath}/CMakeLists.txt)
        foreach(i ${l} )
            get_filename_component(d ${i} DIRECTORY)
            get_dir_targetList(TL ${d})
            list(APPEND target_list ${TL})
        endforeach()
    else()
        get_directory_property(target_list DIRECTORY ${dirPath} BUILDSYSTEM_TARGETS)
    endif()

    set(${_targetList} ${target_list} PARENT_SCOPE)

endfunction()


function(get_cDir_targetList _targetList )
    get_dir_targetList(target_list ${CMAKE_CURRENT_SOURCE_DIR})
    set(${_targetList} ${target_list} PARENT_SCOPE)

endfunction()

function(bs_get_project_targetList)#need to use with bs_add_exe、bs_add_lib



endfunction()


# require predefineVar.cmake included previous to init BS_ARCH var
function(appendArch target_name)

	add_custom_command(TARGET ${target_name} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E rename
	${CMAKE_CURRENT_BINARY_DIR}/${target_name}.dll
	${CMAKE_CURRENT_BINARY_DIR}/${target_name}.${BS_ARCH}.dll
	)
endfunction()
