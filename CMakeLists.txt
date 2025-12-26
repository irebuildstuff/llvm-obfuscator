cmake_minimum_required(VERSION 3.13)
project(LLVMObfuscator VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(LLVM_OBFUSCATOR_BUILD_TESTS "Build tests" ON)
option(LLVM_OBFUSCATOR_BUILD_EXAMPLES "Build examples" ON)
option(LLVM_OBFUSCATOR_USE_SYSTEM_LLVM "Use system LLVM instead of bundled" ON)
option(LLVM_OBFUSCATOR_ENABLE_WARNINGS "Enable compiler warnings" ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required dependencies before LLVM
# Try to find ZLIB using CONFIG mode first (modern CMake), fallback to module mode
find_package(ZLIB QUIET CONFIG)
if(NOT ZLIB_FOUND)
    find_package(ZLIB REQUIRED)
    # Create imported target if it doesn't exist (older CMake versions)
    if(NOT TARGET ZLIB::ZLIB)
        add_library(ZLIB::ZLIB INTERFACE IMPORTED)
        set_target_properties(ZLIB::ZLIB PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${ZLIB_LIBRARIES}"
        )
    endif()
endif()

# Find LLVM
if(LLVM_OBFUSCATOR_USE_SYSTEM_LLVM)
    # Try to find LLVM in standard locations
    find_package(LLVM QUIET CONFIG)
    
    if(NOT LLVM_FOUND)
        # Try common LLVM installation paths
        set(LLVM_SEARCH_PATHS
            "C:/Program Files/LLVM"
            "C:/Program Files (x86)/LLVM"
            "/usr/local/llvm"
            "/usr/lib/llvm-18"
            "/usr/lib/llvm-17"
            "/usr/lib/llvm-16"
            "/usr/lib/llvm-15"
            "/usr/lib/llvm-14"
            "/usr/lib/llvm-13"
            "/usr/lib/llvm-12"
        )
        
        foreach(search_path ${LLVM_SEARCH_PATHS})
            if(EXISTS "${search_path}/lib/cmake/llvm/LLVMConfig.cmake")
                set(LLVM_DIR "${search_path}/lib/cmake/llvm" CACHE PATH "LLVM cmake directory")
                find_package(LLVM REQUIRED CONFIG)
                break()
            endif()
        endforeach()
    endif()
    
    if(NOT LLVM_FOUND)
        message(FATAL_ERROR "LLVM not found. Please install LLVM or set LLVM_DIR to the LLVM cmake directory")
    endif()
    
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    
    # Include LLVM directories
    include_directories(${LLVM_INCLUDE_DIRS})
    separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
    add_definitions(${LLVM_DEFINITIONS_LIST})
    
    # Get LLVM components
    llvm_map_components_to_libnames(llvm_libs 
        Core 
        Support 
        IRReader 
        TransformUtils
        Analysis
        Passes
        CodeGen
        Target
        ${LLVM_TARGETS_TO_BUILD}
    )
else()
    # Use bundled LLVM headers (if available)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/llvm")
        include_directories(${CMAKE_CURRENT_SOURCE_DIR})
        message(STATUS "Using bundled LLVM headers from ${CMAKE_CURRENT_SOURCE_DIR}/llvm")
    else()
        message(FATAL_ERROR "LLVM headers not found. Run download_headers.bat or set LLVM_OBFUSCATOR_USE_SYSTEM_LLVM=ON")
    endif()
    
    # For bundled headers, we still need to link against LLVM libraries
    # This is a simplified approach - you might need to adjust based on your setup
    if(WIN32)
        set(LLVM_LIB_DIR "C:/Program Files/LLVM/lib" CACHE PATH "LLVM library directory")
    else()
        set(LLVM_LIB_DIR "/usr/lib/llvm-14/lib" CACHE PATH "LLVM library directory")
    endif()
    
    link_directories(${LLVM_LIB_DIR})
    set(llvm_libs LLVMCore LLVMSupport LLVMIRReader LLVMTransformUtils LLVMAnalysis)
endif()

# Compiler warnings
if(LLVM_OBFUSCATOR_ENABLE_WARNINGS)
    if(MSVC)
        add_compile_options(/W4 /WX-)
        # Disable some MSVC warnings
        add_compile_options(/wd4100 /wd4244 /wd4267 /wd4996)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic)
        # Optionally treat warnings as errors
        # add_compile_options(-Werror)
    endif()
endif()

# Platform-specific settings
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DNOMINMAX)
    # Windows-specific LLVM definitions
    add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
    
    # Fix runtime library mismatch - use static runtime to match LLVM
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    
    # Link against Windows libraries for anti-debugging features
    set(PLATFORM_LIBS kernel32 user32 advapi32)
elseif(UNIX)
    # Unix-specific settings
    set(PLATFORM_LIBS pthread dl)
    
    # Position independent code for shared libraries
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# ===== Build ObfuscationPass library =====
add_library(ObfuscationPass
    src/ObfuscationPass.cpp
    include/ObfuscationPass.h
)

target_include_directories(ObfuscationPass
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ObfuscationPass
    PUBLIC
        ${llvm_libs}
        ${PLATFORM_LIBS}
)

# Set library properties
set_target_properties(ObfuscationPass PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/ObfuscationPass.h
)

# ===== Build llvm-obfuscator executable =====
add_executable(llvm-obfuscator
    tools/llvm-obfuscator.cpp
)

target_link_libraries(llvm-obfuscator
    PRIVATE
        ObfuscationPass
        ${llvm_libs}
        ${PLATFORM_LIBS}
)

# ===== Build examples (optional) =====
if(LLVM_OBFUSCATOR_BUILD_EXAMPLES AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples)
    add_subdirectory(examples)
endif()

# ===== Build tests (optional) =====
if(LLVM_OBFUSCATOR_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
endif()

# ===== Installation =====
include(GNUInstallDirs)

# Install library
install(TARGETS ObfuscationPass
    EXPORT LLVMObfuscatorTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install executable
install(TARGETS llvm-obfuscator
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Export targets
install(EXPORT LLVMObfuscatorTargets
    FILE LLVMObfuscatorTargets.cmake
    NAMESPACE LLVMObfuscator::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LLVMObfuscator
)

# Create config file (optional)
include(CMakePackageConfigHelpers)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/LLVMObfuscatorConfig.cmake.in)
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/LLVMObfuscatorConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/LLVMObfuscatorConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LLVMObfuscator
    )
endif()

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/LLVMObfuscatorConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/LLVMObfuscatorConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/LLVMObfuscatorConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LLVMObfuscator
)

# ===== CPack configuration (optional) =====
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_PACKAGE_VENDOR "LLVMObfuscator")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LLVM-based code obfuscation tool")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

    if(WIN32)
        set(CPACK_GENERATOR "ZIP;NSIS")
    elseif(UNIX)
        set(CPACK_GENERATOR "TGZ;DEB;RPM")
    endif()

    include(CPack)
endif()

# ===== Summary =====
message(STATUS "")
message(STATUS "LLVMObfuscator Configuration Summary:")
message(STATUS "  Version:           ${PROJECT_VERSION}")
message(STATUS "  Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build tests:       ${LLVM_OBFUSCATOR_BUILD_TESTS}")
message(STATUS "  Build examples:    ${LLVM_OBFUSCATOR_BUILD_EXAMPLES}")
message(STATUS "  Use system LLVM:   ${LLVM_OBFUSCATOR_USE_SYSTEM_LLVM}")
if(LLVM_FOUND)
    message(STATUS "  LLVM version:      ${LLVM_PACKAGE_VERSION}")
    message(STATUS "  LLVM directory:    ${LLVM_DIR}")
endif()
message(STATUS "")
