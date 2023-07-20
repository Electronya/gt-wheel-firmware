# Macro that list all the source file in the project
macro(listSources rootDir result)
  # message("source root dir: ${rootDir}")
  file(GLOB_RECURSE sourceList ${rootDir}/*.c)
  set(${result} ${sourceList})
endmacro()

# Macro that list all the include directory in the project
macro(listIncludesDir rootDir result)
  # message("include root dir: ${rootDir}")
  file(GLOB children RELATIVE ${rootDir} ${rootDir}/*)
  set(dirList "")
  foreach(child ${children})
    if(IS_DIRECTORY ${rootDir}/${child})
      list(APPEND dirList ${rootDir}/${child})
    endif()
  endforeach()
  set(${result} ${dirList})
endmacro()
