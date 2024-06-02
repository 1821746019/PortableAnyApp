
project(pdx CXX)

# 设置编译器选项
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


#add_compile_options(/W3 )


# 判断是不是64位平台
set(bit 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(bit 64)
endif()

include_directories("include")
# depress the warings for using CRT funcs
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

bs_add_all_subdir_if_exist()

