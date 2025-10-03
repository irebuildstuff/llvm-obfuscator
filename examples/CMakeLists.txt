# Examples CMakeLists.txt
# Build example programs for testing the obfuscator

# Simple C example
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/simple_example.ll
    COMMAND clang -S -emit-llvm ${CMAKE_CURRENT_SOURCE_DIR}/simple_example.c 
            -o ${CMAKE_CURRENT_BINARY_DIR}/simple_example.ll
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/simple_example.c
    COMMENT "Compiling simple_example.c to LLVM IR"
)

# Advanced C++ example
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/advanced_example.ll
    COMMAND clang++ -S -emit-llvm ${CMAKE_CURRENT_SOURCE_DIR}/advanced_example.cpp 
            -o ${CMAKE_CURRENT_BINARY_DIR}/advanced_example.ll -std=c++17
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/advanced_example.cpp
    COMMENT "Compiling advanced_example.cpp to LLVM IR"
)

# Custom target to build all examples
add_custom_target(examples
    DEPENDS 
        ${CMAKE_CURRENT_BINARY_DIR}/simple_example.ll
        ${CMAKE_CURRENT_BINARY_DIR}/advanced_example.ll
)

# Obfuscation targets (optional)
if(TARGET llvm-obfuscator)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/simple_example_obf.ll
        COMMAND llvm-obfuscator 
                ${CMAKE_CURRENT_BINARY_DIR}/simple_example.ll
                -o ${CMAKE_CURRENT_BINARY_DIR}/simple_example_obf.ll
                --cf --str --bogus --loops
        DEPENDS llvm-obfuscator ${CMAKE_CURRENT_BINARY_DIR}/simple_example.ll
        COMMENT "Obfuscating simple_example.ll"
    )
    
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/advanced_example_obf.ll
        COMMAND llvm-obfuscator 
                ${CMAKE_CURRENT_BINARY_DIR}/advanced_example.ll
                -o ${CMAKE_CURRENT_BINARY_DIR}/advanced_example_obf.ll
                --cf --str --bogus --loops --mba --anti-debug
        DEPENDS llvm-obfuscator ${CMAKE_CURRENT_BINARY_DIR}/advanced_example.ll
        COMMENT "Obfuscating advanced_example.ll"
    )
    
    add_custom_target(obfuscated_examples
        DEPENDS 
            ${CMAKE_CURRENT_BINARY_DIR}/simple_example_obf.ll
            ${CMAKE_CURRENT_BINARY_DIR}/advanced_example_obf.ll
    )
endif()