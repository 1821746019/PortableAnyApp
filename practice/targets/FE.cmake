
get_filename_component(currFileStem ${CMAKE_CURRENT_LIST_FILE} NAME_WLE)
set(currTarget "_practice-${currFileStem}")



set(dstExePath
    [[D:\environment\win_reverse\Fiddler Everywhere cracked\FE_v6\App\Fiddler Everywhere.exe]]
)
set(args "")
get_filename_component(dstExeDir "${dstExePath}" DIRECTORY)
get_filename_component(appHomeDir ${dstExeDir} DIRECTORY)

bs_add_lib(${currTarget} SHARED _.cpp)
set(extList ElectronCracker)
set(booter extBoot)
set(depList ${booter} ${extList})
target_link_libraries(${currTarget} PRIVATE ${depList})

if(appHomeDir)
  message("appHomeDir is set")
  add_custom_command(TARGET ${currTarget} POST_BUILD
      # copy the extBoot.dll to the target directory
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:${booter}>
      ${appHomeDir}/App/version.dll
  )
  # 
  foreach(e ${extList})
      add_custom_command(TARGET ${currTarget} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${e}>"
          "${appHomeDir}/Ext/$<TARGET_FILE_NAME:${e}>"
      )
  endforeach()
  
else()
  message("appHomeDir is not set")
  add_custom_command(TARGET ${currTarget} POST_BUILD
      # copy the extBoot.dll to the target directory
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:${booter}>
      ${dstExeDir}/version.dll
  )
  # 
  foreach(e ${extList})
      add_custom_command(TARGET ${currTarget} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${e}>"
          "${appHomeDir}/Ext/$<TARGET_FILE_NAME:${e}>"

      )
  endforeach()
endif()
# update the launch.vs.json
update_launch_vs_json(${currTarget} ${dstExePath}
    ARGS ${args}
)