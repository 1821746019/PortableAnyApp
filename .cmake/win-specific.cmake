function(requireAdminForExe exe_name)
	# 告诉链接器嵌入并启用 manifest
	set_target_properties(${exe_name} PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")
endfunction()