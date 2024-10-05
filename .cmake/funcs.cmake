

function(bs_add_all_subdir )

	# include all CMakeLists.txt in dirs in current path
	file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
	foreach(child ${children})
		if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
			add_subdirectory(${child})
		endif()
	endforeach()
endfunction()

function(bs_add_all_subdir_if_exist)
    # 扫描当前路径下的所有项
    file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
    foreach(child ${children})
        # 对于每一个项，检查是否为目录且该目录下存在CMakeLists.txt
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt")
            # 添加子目录到构建中
            add_subdirectory(${child})
        endif()
    endforeach()
endfunction()


function(bs_get_files_paths _file_list)
    set(options ABSOLUTE_PATH)
    set(oneValueArgs)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    List(APPEND pattern_list ${ARG_UNPARSED_ARGUMENTS})
    set(use_absolute_path FALSE)

    if(ARG_ABSOLUTE_PATH)
        set(use_absolute_path TRUE)
    endif()

    set(file_list)#define a local explicitily,without specify a value, default is empty
    foreach(pattern IN LISTS pattern_list)
        set(match_mode)
        # assume pattern do not have *
        set(file_path ${pattern})
        string(FIND ${pattern} ** begin_idx)

        if(NOT ${begin_idx} EQUAL -1)
            set(match_mode GLOB_RECURSE)
        else()
            string(FIND ${pattern} * begin_idx)

            if(NOT ${begin_idx} EQUAL -1)
                set(match_mode GLOB)
            endif()
        endif()

        # if match_mode set, pattern contains ** or *, write new paths to file_path
        if(match_mode)
            file(${match_mode} file_path RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${pattern})

        endif()
        if(${use_absolute_path})
            get_filename_component(file_path ${file_path} ABSOLUTE)
        endif()

        list(APPEND file_list ${file_path})
    endforeach()

    set(${_file_list} ${file_list} PARENT_SCOPE)
endfunction()

function(bs_get_dirs_paths _ret root)
    # 解析函数参数，这里只定义了ABSOLUTE_PATH作为可能的选项
    set(options ABSOLUTE_PATH)
    set(oneValueArgs "")
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 如果没有设置ABSOLUTE_PATH参数，或者设置为FALSE，cmake_parse_arguments不会定义ARG_ABSOLUTE_PATH变量
    # 因此我们可以通过检查这个变量是否被定义来决定默认值
    set(use_absolute_path FALSE)

    if(ARG_ABSOLUTE_PATH)
        set(use_absolute_path TRUE)
    endif()

    file(GLOB_RECURSE dirs LIST_DIRECTORIES true ${root}/*)
    set(ret)

    foreach(dir ${dirs})
        if(NOT use_absolute_path)
            # 计算相对于root的相对路径
            file(RELATIVE_PATH rel_dir ${root} ${dir})
            list(APPEND ret ${rel_dir})
        else()
            list(APPEND ret ${dir}) # 已经是绝对路径
        endif()
    endforeach()

    # 返回结果
    set(${_ret} ${ret} PARENT_SCOPE)
endfunction()

function(bs_split_srcs _ixx_list _cpp_list )
    set(options )
    set(oneValueArgs )
    set(multiValueArgs)
    cmake_parse_arguments("ARG" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(file_list ${ARG_UNPARSED_ARGUMENTS})

    set(known_module_exts .ixx .cppm)

    set(ixx_list)
    set(cpp_list)

    foreach(i IN LISTS file_list)
        get_filename_component(ext ${i} LAST_EXT )
        set(which_list cpp_list)

        if(${ext} IN_LIST known_module_exts)
            set(which_list ixx_list)
        endif()

        list(APPEND ${which_list} ${i})
    endforeach()

    set(${_ixx_list} ${ixx_list} PARENT_SCOPE)
    set(${_cpp_list} ${cpp_list} PARENT_SCOPE)
endfunction()
# 定义一个函数来找到路径列表中的最长公共前缀
function(find_longest_common_baseDir _common_prefix path_list )
  # 确定列表中第一个路径作为初始前缀
  list(GET path_list 0 first_path)
  #set(prefix ${first_path})
  string(REGEX MATCH "^.*/" prefix ${first_path})
  # 遍历列表中的每个路径
  foreach(path IN LISTS path_list)
    # 当前前缀长度
    string(LENGTH ${prefix} prefix_length)
    
    # 逐字符比较当前路径与当前前缀，寻找最长公共前缀
    while(prefix_length GREATER 0)
      string(SUBSTRING ${path} 0 ${prefix_length} path_prefix)
      string(REGEX MATCH "/$" isEndWithSlash ${path_prefix})
      if(NOT "${path_prefix}" STREQUAL "${prefix}" )
        # 如果不相等，缩短前缀长度
        math(EXPR prefix_length "${prefix_length} - 1")
        string(SUBSTRING ${prefix} 0 ${prefix_length} prefix)
      else()
        # 如果相等，退出循环
        break()
      endif()
    endwhile()
    
    # 如果前缀长度已缩减为零，提前退出
    if(prefix_length EQUAL 0)
      break()
    endif()
  endforeach()

  # 设置输出变量为最终的最长公共前缀
  set(${_common_prefix} ${prefix} PARENT_SCOPE)
endfunction()
function(bs_add_lib name)
    set(lib_type STATIC)
    set(patterns)

    set(known_lib_types STATIC SHARED MODULE INTERFACE OBJECT)

    foreach(arg IN LISTS ARGN)
        if(arg IN_LIST known_lib_types)
            set(lib_type ${arg})
        else()
            list(APPEND patterns ${arg})
        endif()
    endforeach()


    bs_get_files_paths(file_list ${patterns})
    bs_split_srcs(ixx_list cpp_list ${file_list})
    add_library(${name} ${lib_type}  "${cpp_list}" )
    # if( ixx_list)
        # set(abs_ixx_list "")
        # foreach(ixx ${ixx_list})
            # get_filename_component(abs_path ${ixx} ABSOLUTE)
            # list(APPEND abs_ixx_list ${abs_path})
        # endforeach()
        # find_longest_common_baseDir(common_baseDir "${abs_ixx_list}")
        # string(LENGTH ${common_baseDir} common_baseDir_len)
        # string(LENGTH ${CMAKE_CURRENT_SOURCE_DIR} src_dir_len)
        # if(common_baseDir_len GREATER src_dir_len)
            # set(common_baseDir ${CMAKE_CURRENT_SOURCE_DIR})
        # endif()
##BASE_DIRS ${common_baseDir}
        # target_sources(${name} PUBLIC FILE_SET CXX_MODULES  BASE_DIRS ${common_baseDir} FILES ${abs_ixx_list})
    # endif()
	    
    if( ixx_list )
        #寻找最长的baseDir
        set(abs_ixx_list "")
        foreach(f ${ixx_list})
            get_filename_component(abs_file ${f} ABSOLUTE)
            list(APPEND abs_ixx_list ${abs_file})
        endforeach()
        find_longest_common_baseDir(common_baseDir "${abs_ixx_list}")
       #BASE_DIRS ${common_baseDir}
        target_sources(${name} PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${common_baseDir} FILES ${abs_ixx_list})
    endif()
endfunction()

function(bs_add_exe name)
    set(exe_type) # null represent normal console app
    set(patterns)

    set(known_exe_types WIN32)

    foreach(arg IN LISTS ARGN)
        if(arg IN_LIST known_exe_types)
            set(exe_type ${arg})
        else()
            list(APPEND patterns ${arg})
        endif()
    endforeach()

    bs_get_files_paths(file_list ${patterns})
    bs_split_srcs(ixx_list cpp_list ${file_list})
    add_executable(${name} ${exe_type} ${cpp_list} ${ixx_list})
    
    if( ixx_list )
        #寻找最长的baseDir
        set(abs_ixx_list "")
        foreach(f ${ixx_list})
            get_filename_component(abs_file ${f} ABSOLUTE)
            list(APPEND abs_ixx_list ${abs_file})
        endforeach()
        find_longest_common_baseDir(common_baseDir "${abs_ixx_list}")
       #BASE_DIRS ${common_baseDir}
        target_sources(${name} PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${common_baseDir} FILES ${abs_ixx_list})
    endif()
endfunction()

# 将宏bs_get_dirs改写为函数
function(bs_get_dirs _result _root)
    file(GLOB_RECURSE dirs RELATIVE ${_root} LIST_DIRECTORIES true ${_root}/*)
    set(result_dirs)
    foreach(dir ${dirs})
        if(IS_DIRECTORY "${_root}/${dir}")
            list(APPEND result_dirs "${dir}")
        endif()
    endforeach()
    set(${_result} ${result_dirs} PARENT_SCOPE)
endfunction()

# 将宏bs_get_targets_by_dir改写为函数
function(bs_get_targets_by_dir _result _dir)
    get_property(_target DIRECTORY ${_dir} PROPERTY BUILDSYSTEM_TARGETS)
    set(${_result} ${_target} PARENT_SCOPE)
endfunction()

# 将宏bs_get_targets改写为函数
function(bs_get_targets _result _root_dir)
    bs_get_dirs(all_dir ${_root_dir})
    list(LENGTH all_dir len)
    if(NOT ${len})
        set(all_dir ${_root_dir})
    endif()
    message(WARNING "root_dir is ${_root_dir}")
    message(WARNING "all_dir is ${all_dir}")
    set(result_targets)
    foreach(dir IN LISTS all_dir)
        bs_get_targets_by_dir(_target "${_root_dir}/${dir}")
        if(_target)
            list(APPEND result_targets ${_target})
        endif()
    endforeach()
    set(${_result} ${result_targets} PARENT_SCOPE)
endfunction()

# 将宏get_all_targets_c改写为函数
function(get_all_targets_c _targetList)
    get_directory_property(local_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} BUILDSYSTEM_TARGETS)
    foreach(_target IN LISTS local_targets)
        message(STATUS "Found target: ${_target}")
    endforeach()
    set(${_targetList} ${local_targets} PARENT_SCOPE)
endfunction()

# 将宏get_all_targets改写为函数
function(get_all_targets _cmake_list_dir _targetList)
    get_directory_property(local_targets DIRECTORY ${_cmake_list_dir} BUILDSYSTEM_TARGETS)
    foreach(_target IN LISTS local_targets)
        message(STATUS "Found target in ${_cmake_list_dir}: ${_target}")
    endforeach()
    set(${_targetList} ${local_targets} PARENT_SCOPE)
endfunction()

# 将宏get_project_targets改写为函数
function(get_project_targets project_dir)
    file(GLOB_RECURSE _cmakeListsFiles "${project_dir}/**/CMakeLists.txt")
    message(WARNING "cmake_lists_files is ${_cmakeListsFiles}")
    foreach(_file IN LISTS _cmakeListsFiles)
        get_filename_component(_dir ${_file} DIRECTORY)
        get_all_targets(${_dir} targetList)
    endforeach()
endfunction()
