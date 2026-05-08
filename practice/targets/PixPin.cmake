get_filename_component(currFileStem ${CMAKE_CURRENT_LIST_FILE} NAME_WLE)
set(currTarget "_practice-${currFileStem}")

set(dstExePath [[D:\Program\Productivity\Utils\PixPin\PixPin_v2.2\App\PixPin.exe]])
set(args "")
get_filename_component(dstExeDir "${dstExePath}" DIRECTORY)
get_filename_component(appHomeDir ${dstExeDir} DIRECTORY)
bs_add_lib(${currTarget} SHARED _.cpp)
set(extList HijackResponseQt5)
set(depList ${extList})
target_link_libraries(${currTarget} PRIVATE ${depList})
add_custom_command(TARGET ${currTarget} POST_BUILD
    # copy the extBoot.dll to the target directory
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:extBoot>
    "${appHomeDir}/App/version.dll"

)
foreach(e ${extList})
    add_custom_command(TARGET ${currTarget} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${e}>"
        "${appHomeDir}/Ext/$<TARGET_FILE_NAME:${e}>"

    )
endforeach()

# update the launch.vs.json
update_launch_vs_json(${currTarget} ${dstExePath}
    ARGS ${args}
)