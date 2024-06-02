set(lib_name uniPortabler)
bs_add_lib(${lib_name} SHARED "*.ixx"  "*.cpp" "*.def"  )
#
set_target_properties(${lib_name}  PROPERTIES OUTPUT_NAME "version")
#bs_get_files_paths(src_list )



target_link_libraries(${lib_name} PRIVATE versionForged fs_core common)

add_custom_command(TARGET ${lib_name} POST_BUILD COMMAND
${CMAKE_COMMAND} -E copy 
$<TARGET_FILE:${lib_name}> 
"D:\\Program Files\\安卓模拟\\LDPlayer_v9.0.71\\App"
"D:\\Users\\Administrator\\Desktop\\repository\\离线网页\\##miscellaneous\\#windows相关\\打开资源管理器让用户选择文件\\App32"
#"D:\\Users\\Administrator\\Desktop\\repository\\离线网页\\##miscellaneous\\#windows相关\\打开资源管理器让用户选择文件\\App"

)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
