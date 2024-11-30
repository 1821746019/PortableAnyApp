
set(currTarget "WebStormPractice")
bs_add_lib(${currTarget} SHARED _.cpp)
set(extList uniPortabler LifeCycleMgr)
set(depList extBoot ${extList})
target_link_libraries(${currTarget} PRIVATE ${depList})
add_custom_command(TARGET ${currTarget} POST_BUILD
# copy the extBoot.dll to the target directory
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:extBoot>
		"D:/environment/IDE/JetBrains/WebStorm/WebStorm_Portable/App/bin/version.dll"

)
foreach(e ${extList})
	add_custom_command(TARGET ${currTarget}   
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${e}>
			"D:/environment/IDE/JetBrains/WebStorm/WebStorm_Portable/Ext/$<TARGET_FILE_NAME:${e}>"
	)
endforeach()