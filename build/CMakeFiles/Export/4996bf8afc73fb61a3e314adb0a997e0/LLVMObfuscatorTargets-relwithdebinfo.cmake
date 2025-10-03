#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LLVMObfuscator::ObfuscationPass" for configuration "RelWithDebInfo"
set_property(TARGET LLVMObfuscator::ObfuscationPass APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(LLVMObfuscator::ObfuscationPass PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/ObfuscationPass.lib"
  )

list(APPEND _cmake_import_check_targets LLVMObfuscator::ObfuscationPass )
list(APPEND _cmake_import_check_files_for_LLVMObfuscator::ObfuscationPass "${_IMPORT_PREFIX}/lib/ObfuscationPass.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
