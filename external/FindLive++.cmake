# FindLib.cmake
# This is a CMake module to locate the Lib library

# Define the package name
set(PACKAGE_NAME "Live++")

# Define the package version, if applicable
set(PACKAGE_VERSION "1.0")

# Try to locate the library and its include directories
find_path(${PACKAGE_NAME}_INCLUDE_DIR
  NAMES LPP_API_x64_CPP.h  # Replace with the main header file of the library
  PATHS
    D:/environment/c++/LivePP/API/x64  # Adjust this path according to your directory structure
  DOC "The directory where the ${PACKAGE_NAME} headers are located"
)

#find_library(${PACKAGE_NAME}_LIBRARY
#  NAMES lib  # Replace with the library name without the 'lib' prefix and file extension
#  PATHS
#    ${CMAKE_CURRENT_LIST_DIR}/../lib  # Adjust this path according to your directory structure
#    /usr/local/lib
#    /usr/lib
#  DOC "The ${PACKAGE_NAME} library"
#)

# If the library and include directories were found, set the _FOUND variable to true
if(${PACKAGE_NAME}_INCLUDE_DIR )
  set(${PACKAGE_NAME}_FOUND TRUE)
else()
  set(${PACKAGE_NAME}_FOUND FALSE)
endif()

# Provide user-friendly messages
if(${PACKAGE_NAME}_FOUND)
  message(STATUS "${PACKAGE_NAME} found: ${${PACKAGE_NAME}_LIBRARY}")
  message(STATUS "${PACKAGE_NAME} include dir: ${${PACKAGE_NAME}_INCLUDE_DIR}")
else()
  message(STATUS "${PACKAGE_NAME} not found")
endif()

# Set the results to be used by other CMake scripts
if(${PACKAGE_NAME}_FOUND)
  set(${PACKAGE_NAME}_INCLUDE_DIRS ${${PACKAGE_NAME}_INCLUDE_DIR})
  #set(${PACKAGE_NAME}_LIBRARIES ${${PACKAGE_NAME}_LIBRARY})
endif()

# Mark the cache variables as advanced to keep the cache clean
mark_as_advanced(${PACKAGE_NAME}_INCLUDE_DIR ${PACKAGE_NAME}_LIBRARY)
