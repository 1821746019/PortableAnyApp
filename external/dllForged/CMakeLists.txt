set(dlls version;)

foreach(dll ${dlls})

	bs_add_lib("${dll}Forged"  "${dll}Func.def" "${dll}Func.ixx" )

endforeach()