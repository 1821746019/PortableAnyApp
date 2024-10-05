set(dlls version;)

foreach(dll ${dlls})

	bs_add_lib("${dll}Forged" STATIC    "${dll}Func.ixx" )
	target_sources("${dll}Forged" PUBLIC "${dll}Func.def")
	#set_target_properties("${dll}Forged" PROPERTIES COMPILE_FLAGS_RELWITHDEBINFO "-Os")
	# 为 MSVC 编译器的 my_target 设置最小大小优化
	if(${CMAKE_BUILD_TYPE} MATCHES "^Rel")
		target_compile_options("${dll}Forged" PRIVATE /Os)

	endif()
	target_link_libraries(${dll}Forged std)
endforeach()

#bs_add_lib("kernel32new" "kernel32new.def" "kernel32new.ixx")
#target_sources(${name} PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${common_baseDir} FILES ${abs_ixx_list})

#target_sources(kernel32Forged PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES "kernel32Func_undoced.ixx" "kernel32Func_mannual.ixx")
