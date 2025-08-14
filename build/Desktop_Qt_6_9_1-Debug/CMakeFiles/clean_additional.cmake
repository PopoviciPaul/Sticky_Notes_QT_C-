# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Sticky_Notes_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Sticky_Notes_autogen.dir/ParseCache.txt"
  "Sticky_Notes_autogen"
  )
endif()
