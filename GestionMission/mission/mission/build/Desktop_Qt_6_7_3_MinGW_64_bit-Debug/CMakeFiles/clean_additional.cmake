# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\mission_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\mission_autogen.dir\\ParseCache.txt"
  "mission_autogen"
  )
endif()
