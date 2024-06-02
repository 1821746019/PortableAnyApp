include_directories("../include")
set(exe_name "fs_test")
bs_add_exe(${exe_name} WIN32 "*.cpp" "*.ixx")
target_link_libraries(${exe_name} PUBLIC  ole32 comdlg32 version winmm)
