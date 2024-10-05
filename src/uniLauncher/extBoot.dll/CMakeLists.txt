

set(dll_name extBoot)

bs_add_lib(${dll_name} SHARED
	*.cpp *.ixx )

appendArch(${dll_name})
target_link_libraries(${dll_name} versionForged)

add_custom_command(TARGET ${dll_name} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_CURRENT_BINARY_DIR}/${dll_name}.${BS_ARCH}.dll
	"${CMAKE_CURRENT_BINARY_DIR}/.."
)