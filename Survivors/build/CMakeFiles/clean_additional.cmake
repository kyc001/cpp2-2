# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Survivors_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Survivors_autogen.dir\\ParseCache.txt"
  "Survivors_autogen"
  )
endif()
