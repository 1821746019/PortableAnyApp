# CMakeList.txt : CMake project for DynamicAndStatic, include source and define
# project specific logic here.
#

# Add source to this project's executable.

#set(exe_name "DynamicAndStatic")
#add_executable (${exe_name} "DynamicAndStatic.cpp" "DynamicAndStatic.h")


#find_package(polyhook_2 CONFIG REQUIRED)
#target_link_libraries(${exe_name} PRIVATE PolyHook_2::PolyHook_2)

message(WARNING "vcpkg_pkg.cmake")
function(execInOtherTriplet triplet proc)
    # 将VCPKG_TARGET_TRIPLET的值备份到VCPKG_TARGET_TRIPLET_BACKUP
    set(VCPKG_TARGET_TRIPLET_BACKUP "${VCPKG_TARGET_TRIPLET}" CACHE STRING "Backup of the original VCPKG_TARGET_TRIPLET" FORCE)
    set(VCPKG_TARGET_TRIPLET "${triplet}" CACHE STRING "Set VCPKG target triplet" FORCE)


    # 设置VCPKG_TARGET_TRIPLET的新值为传入的triplet

    set(to_replace_list 
    "CMAKE_FIND_ROOT_PATH"
    "CMAKE_LIBRARY_PATH"
    "CMAKE_PREFIX_PATH"
    "CMAKE_PROGRAM_PATH"
    "VCPKG_CMAKE_FIND_ROOT_PATH"
    )
    foreach(to_replace ${to_replace_list})

      set(${to_replace}_BACKUP "${${to_replace}}" CACHE STRING  "Backup of the original one " FORCE)
      # 替换操作
      string(REPLACE "/${VCPKG_TARGET_TRIPLET_BACKUP}" "/${VCPKG_TARGET_TRIPLET}" ${to_replace} "${${to_replace}}")
      #string(REPLACE "${VCPKG_TARGET_TRIPLET_BACKUP}/" "${VCPKG_TARGET_TRIPLET}/" VCPKG_CMAKE_FIND_ROOT_PATH "${VCPKG_CMAKE_FIND_ROOT_PATH}")

      # 输出结果以检查替换是否成功
      message(STATUS "Updated ${to_replace}: ${${to_replace}}")
    endforeach()




    # 执行传入的proc指令
    cmake_language(CALL ${proc})


    foreach(to_replace ${to_replace_list})

    # 恢复原来的triplet
    #set(VCPKG_TARGET_TRIPLET "${VCPKG_TARGET_TRIPLET_BACKUP}" CACHE STRING "Restore original VCPKG_TARGET_TRIPLET" FORCE)
    set(${to_replace}	 "${${to_replace}_BACKUP}" CACHE STRING "Restore original ${to_replace}	" FORCE)

    endforeach()

    set(VCPKG_TARGET_TRIPLET "${VCPKG_TARGET_TRIPLET_BACKUP}" CACHE STRING "Set VCPKG target triplet" FORCE)

endfunction()


