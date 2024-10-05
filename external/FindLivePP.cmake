# FindLivePP.cmake
# This module locates the Live++ SDK and sets the necessary variables.
# It sets the following variables:
#   LivePP_FOUND        - Set to TRUE if Live++ is found
#   LivePP_INCLUDE_DIRS - The directories containing Live++ headers
#   LivePP_LIBRARIES    - The libraries to link against

#set(base_dir "API/x64/")
find_path(LivePP_INCLUDE_DIR LPP_API.h
  PATHS
    ${CMAKE_CURRENT_LIST_DIR}/external/LivePP/API/x64
)

find_library(LivePP_AGENT_LIB
  NAMES LPP_Agent_x64_CPP
  PATHS
    ${CMAKE_CURRENT_LIST_DIR}/external/LivePP/Agent/x64
)

find_library(LivePP_BROKER_LIB
  NAMES LPP_Broker_x64_CPP
  PATHS
    ${CMAKE_CURRENT_LIST_DIR}/external/LivePP/Broker
)

if (LivePP_INCLUDE_DIR AND LivePP_AGENT_LIB AND LivePP_BROKER_LIB)
  set(LivePP_FOUND TRUE)
  set(LivePP_INCLUDE_DIRS ${LivePP_INCLUDE_DIR})
  set(LivePP_LIBRARIES ${LivePP_AGENT_LIB} ${LivePP_BROKER_LIB})
else()
  set(LivePP_FOUND FALSE)
endif()

mark_as_advanced(LivePP_INCLUDE_DIR LivePP_AGENT_LIB LivePP_BROKER_LIB)
