if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BS_ARCH "x64")
else()
    set(BS_ARCH "x86")
    
endif()