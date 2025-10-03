# Source directory CMakeLists.txt
add_library(ObfuscationPass
    ObfuscationPass.cpp
)

target_include_directories(ObfuscationPass
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(ObfuscationPass
    ${llvm_libs}
)
