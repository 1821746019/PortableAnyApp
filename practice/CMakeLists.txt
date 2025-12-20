function(update_launch_vs_json targetName exePath)
    # Parse optional arguments
    set(options "")
    set(oneValueArgs "ARGS" "CURRENT_DIR")
    set(multiValueArgs "")
    cmake_parse_arguments(UPDATE_LAUNCH_VS_JSON "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Generate projectTarget
    file(RELATIVE_PATH relPath ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    set(projectTarget "${targetName}.dll (${relPath}/${targetName}.dll)")

    # Use double backslashes uniformly
    string(REPLACE "/" "\\" projectTarget "${projectTarget}")
    message("projectTarget: ${projectTarget}")

    # Prepare optional arguments
    set(argsFlag "")
    if(UPDATE_LAUNCH_VS_JSON_ARGS)
        set(argsFlag "-a ${UPDATE_LAUNCH_VS_JSON_ARGS}")
    endif()

    set(currentDirFlag "")
    if(UPDATE_LAUNCH_VS_JSON_CURRENT_DIR)
        set(currentDirFlag "-c ${UPDATE_LAUNCH_VS_JSON_CURRENT_DIR}")
    endif()
    execute_process(COMMAND  python ${PROJECT_SOURCE_DIR}/tools/update_launch_vs_json.py 
            ${PROJECT_SOURCE_DIR}/.vs/launch.vs.json 
            ${projectTarget} 
            ${exePath} 
            ${argsFlag} 
            ${currentDirFlag})
    # Execute the Python script
    # add_custom_command(TARGET ${targetName} PRE_BUILD
    #     COMMAND python ${PROJECT_SOURCE_DIR}/tools/update_launch_vs_json.py
    #     ${PROJECT_SOURCE_DIR}/.vs/launch.vs.json
    #     ${projectTarget}
    #     ${exePath}
    #     ${argsFlag}
    #     ${currentDirFlag}
    #     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    # )
endfunction()

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

set(currTarget "_uToolsPractice")
set(dstExePath [[D:\Program\Productivity\Utils\uTools_4.4.1_Portable\App\uTools.exe]])
set(args [[--portable --user-data-dir="D:\Program\Productivity\Utils\uTools_4.4.1_Portable\Data\user_data"]])
bs_add_lib(${currTarget} SHARED _.cpp)
set(extList RegCore)
set(depList extBoot ${extList})
target_link_libraries(${currTarget} PRIVATE ${depList})
add_custom_command(TARGET ${currTarget} POST_BUILD
    # copy the extBoot.dll to the target directory
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:extBoot>
    "D:/Program/Productivity/Utils/uTools_4.4.1_Portable/App/version.dll"

)
foreach(e ${extList})
    add_custom_command(TARGET ${currTarget} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${e}>"
        "D:/Program/Productivity/Utils/uTools_4.4.1_Portable/Ext/$<TARGET_FILE_NAME:${e}>"

    )
endforeach()
# update the launch.vs.json
update_launch_vs_json(${currTarget} ${dstExePath}
    ARGS ${args}
)





set(currTarget "VSCodePractice")
set(dstExeDir "D:/environment/IDE/VSCode/VSCode_v1.83_32Bit_")
set(appHomeDir)
bs_add_lib(${currTarget} SHARED _.cpp)
set(extList)
set(depList UniqueWindowGroup ${extList})
target_link_libraries(${currTarget} PRIVATE ${depList})
add_custom_command(TARGET ${currTarget} POST_BUILD
    # copy the extBoot.dll to the target directory
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:UniqueWindowGroup>
    $<IF:$<BOOL:${dstExeDir}>,"${dstExeDir}/version.dll","${appHomeDir}/App/version.dll">
)

foreach(e ${extList})
    add_custom_command(TARGET ${currTarget} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${e}>"
        "${appHomeDir}/Ext/$<TARGET_FILE_NAME:${e}>"

    )
endforeach()

set(currTarget "_practice-reg")
set(dstExePath
    #[[D:\Program\Downloader\IDM\IDM_v6.38\IDMan.exe]]
    [[D:\Program\OSEnhance\RegistryFinder_2.59.0_64bit_Green\RegistryFinder.exe]]
)
set(args "")
get_filename_component(dstExeDir "${dstExePath}" DIRECTORY)
set(appHomeDir)
bs_add_lib(${currTarget} SHARED _.cpp)
set(extList)
set(booter RegCore)
set(depList ${booter} ${extList})
target_link_libraries(${currTarget} PRIVATE ${depList})
add_custom_command(TARGET ${currTarget} POST_BUILD
    # copy the extBoot.dll to the target directory
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:${booter}>
    $<IF:$<BOOL:${dstExeDir}>,"${dstExeDir}/version.dll","${appHomeDir}/App/version.dll">
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


set(currTarget "_practice-IDM")
set(dstExePath
    [[D:\Program\Downloader\IDM\IDM_v6.42\IDMan.exe]]
)
set(args "")
get_filename_component(dstExeDir "${dstExePath}" DIRECTORY)
set(appHomeDir)
bs_add_lib(${currTarget} SHARED _.cpp)
set(extList)
set(booter RegCore)
set(depList ${booter} ${extList})
target_link_libraries(${currTarget} PRIVATE ${depList})
add_custom_command(TARGET ${currTarget} POST_BUILD
    # copy the extBoot.dll to the target directory
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:${booter}>
    $<IF:$<BOOL:${dstExeDir}>,"${dstExeDir}/version.dll","${appHomeDir}/App/version.dll">
)
# 
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


set(currTarget "_practice-wps")
set(dstExePath
    [[D:\Program Files\WPS\WPS_Portable\App\wps.exe]]
)
set(args "")
get_filename_component(dstExeDir "${dstExePath}" DIRECTORY)
set(appHomeDir)
bs_add_lib(${currTarget} SHARED _.cpp)
set(extList)
set(booter RegCore)
set(depList ${booter} ${extList})
target_link_libraries(${currTarget} PRIVATE ${depList})
add_custom_command(TARGET ${currTarget} POST_BUILD
    # copy the extBoot.dll to the target directory
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:${booter}>
    $<IF:$<BOOL:${dstExeDir}>,"${dstExeDir}/version.dll","${appHomeDir}/App/version.dll">
)
# 
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

# get all the .cmake files in the targets directory
#file(GLOB targetsCMake RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cmake")
file(GLOB targetsCMake RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/targets" "${CMAKE_CURRENT_SOURCE_DIR}/targets/*.cmake")

message("targetsCMake: ${targetsCMake}")
foreach(cmakeFile ${targetsCMake})

    include("${CMAKE_CURRENT_SOURCE_DIR}/targets/${cmakeFile}")
endforeach()