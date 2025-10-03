# Tools directory CMakeLists.txt
add_executable(llvm-obfuscator
    llvm-obfuscator.cpp
)

target_include_directories(llvm-obfuscator
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(llvm-obfuscator
    ObfuscationPass
    ${llvm_libs}
)
