set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)
link_libraries(  Qt5::Widgets Qt5::Core Qt5::Gui)

set(lib_name AdminOps)
bs_add_lib(${lib_name} SHARED *.ixx)


set(exe_name AdminOpsService)
bs_add_exe(${exe_name} WIN32 service/*.ixx )

requireAdminForExe(${exe_name})
