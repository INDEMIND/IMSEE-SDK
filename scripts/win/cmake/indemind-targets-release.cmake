#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "indemind" for configuration "Release"
set_property(TARGET indemind APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(indemind PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${INDEMINDS_SDK_ROOT}/lib/indemind.lib"
  IMPORTED_LOCATION_RELEASE "${INDEMINDS_SDK_ROOT}/bin/indemind.dll"
  )

# Import target "indemind" for configuration "Debug"
set_property(TARGET indemind APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(indemind PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${INDEMINDS_SDK_ROOT}/lib/indemindd.lib"
  IMPORTED_LOCATION_DEBUG "${INDEMINDS_SDK_ROOT}/bin/indemindd.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS indemind )
list(APPEND _IMPORT_CHECK_FILES_FOR_indemind
  "${INDEMINDS_SDK_ROOT}/lib/indemind.lib" "${INDEMINDS_SDK_ROOT}/bin/indemind.dll"
  "${INDEMINDS_SDK_ROOT}/lib/indemindd.lib" "${INDEMINDS_SDK_ROOT}/bin/indemindd.dll"
  )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
