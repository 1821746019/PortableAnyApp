
function(renameToArchDll target_name)
	add_custom_command(TARGET ${target_name} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E rename
	${CMAKE_CURRENT_BINARY_DIR}/${target_name}.dll
	${CMAKE_CURRENT_BINARY_DIR}/${target_name}.${BS_ARCH}.dll
	)

endfunction()

BS_ADD_ALL_SUBDIR_IF_EXIST()

set(exe_name  uniLauncher)
bs_add_exe( ${exe_name} WIN32
	*.cpp *.ixx)
add_dependencies(${exe_name} extBoot)



