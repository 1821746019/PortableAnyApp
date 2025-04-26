
get_filename_component(currFileStem ${CMAKE_CURRENT_LIST_FILE} NAME_WLE)
set(currTarget "_practice-${currFileStem}")



set(dstExePath
    [[D:\Program Files\Browers\Chrome\_clean_Chrome_v117\App\chrome.exe]]
)
set(args [[--portable "--user-data-dir=D:\Program Files\Browers\Chrome\_clean_Chrome_v117\Data\user_data" "--disk-cache-dir=D:\Program Files\Browers\Chrome\_clean_Chrome_v117\Data\cache" --disable-features=RendererCodeIntegrity --no-first-run --disable-logging "--no-report-upload " --disable-background-networking]])
get_filename_component(dstExeDir "${dstExePath}" DIRECTORY)
get_filename_component(appHomeDir ${dstExeDir} DIRECTORY)
#do not use the appHomeDir
set(appHomeDir )
bs_add_lib(${currTarget} SHARED _.cpp)
set(extList )
set(booter hookString)
set(depList ${booter} ${extList})

target_link_libraries(${currTarget} PRIVATE ${depList})

# update the launch.vs.json
update_launch_vs_json(${currTarget} ${dstExePath}
    ARGS ${args}
)

if(NOT depList)
  message("depList is empty")
  return()
endif()


if(appHomeDir)
  message("appHomeDir is set")
  add_custom_command(TARGET ${currTarget} POST_BUILD
      # copy the extBoot.dll to the target directory
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:${booter}>
      ${appHomeDir}/App/winmm.dll
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
      ${dstExeDir}/winmm.dll
  )
  # 
  foreach(e ${extList})
      add_custom_command(TARGET ${currTarget} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${e}>"
          "${appHomeDir}/Ext/$<TARGET_FILE_NAME:${e}>"

      )
  endforeach()
endif()
