file(GLOB_RECURSE file_list RELATIVE ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/*.cmake")

foreach(f IN LISTS file_list)
if(NOT f  STREQUAL ".cmake")
	include(${CMAKE_CURRENT_LIST_DIR}/${f})
endif()
endforeach()