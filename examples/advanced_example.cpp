/**
 * Advanced Example - C++ program with complex features
 * Demonstrates obfuscation of OOP code and advanced algorithms
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

// Proprietary encryption class
class CryptoEngine {
private:
    static constexpr uint32_t SEED = 0xDEADBEEF;
    uint32_t state;
    
    uint32_t rotateLeft(uint32_t value, int shift) {
        return (value << shift) | (value >> (32 - shift));
    }
    
public:
    CryptoEngine() : state(SEED) {}
    
    std::vector<uint8_t> encrypt(const std::string& data) {
        std::vector<uint8_t> encrypted;
        encrypted.reserve(data.size());
        
        for (char c : data) {
            state = rotateLeft(state, 7) ^ 0x13371337;
            uint8_t encrypted_byte = c ^ (state & 0xFF);
            encrypted.push_back(encrypted_byte);
        }
        
        return encrypted;
    }
    
    std::string decrypt(const std::vector<uint8_t>& data) {
        std::string decrypted;
        state = SEED; // Reset state
        
        for (uint8_t byte : data) {
            state = rotateLeft(state, 7) ^ 0x13371337;
            char decrypted_char = byte ^ (state & 0xFF);
            decrypted.push_back(decrypted_char);
        }
        
        return decrypted;
    }
};

// Protected algorithm implementation
class SecretProcessor {
private:
    int secret_key;
    std::vector<int> lookup_table;
    
    void initializeLookupTable() {
        lookup_table.resize(256);
        for (int i = 0; i < 256; i++) {
            lookup_table[i] = (i * secret_key) ^ (secret_key >> (i % 8));
        }
    }
    
public:
    SecretProcessor(int key) : secret_key(key) {
        initializeLookupTable();
    }
    
    int process(int input) {
        // Complex proprietary algorithm
        int result = input;
        
        for (int round = 0; round < 3; round++) {
            result = lookup_table[result & 0xFF];
            result ^= (result >> 8) * secret_key;
            result = rotateRight(result, 5);
        }
        
        return result;
    }
    
private:
    int rotateRight(int value, int shift) {
        return (value >> shift) | (value << (32 - shift));
    }
};

// Anti-debugging check
bool isDebuggerPresent() {
    #ifdef _WIN32
        // Windows-specific check
        return false; // Simplified for example
    #else
        // Linux-specific check
        return false; // Simplified for example
    #endif
}

int main() {
    std::cout << "Advanced Protected Application\n";
    
    // Anti-debugging
    if (isDebuggerPresent()) {
        std::cout << "Debugger detected! Exiting...\n";
        return -1;
    }
    
    // Encryption demo
    CryptoEngine crypto;
    std::string sensitive_data = "This is confidential information!";
    
    auto encrypted = crypto.encrypt(sensitive_data);
    std::cout << "Data encrypted (" << encrypted.size() << " bytes)\n";
    
    auto decrypted = crypto.decrypt(encrypted);
    std::cout << "Data decrypted: " << decrypted << "\n";
    
    // Secret processing
    SecretProcessor processor(0x42424242);
    std::vector<int> inputs = {100, 200, 300, 400, 500};
    
    std::cout << "\nProcessing data:\n";
    for (int input : inputs) {
        int output = processor.process(input);
        std::cout << "  " << input << " -> " << output << "\n";
    }
    
    std::cout << "\nApplication completed successfully.\n";
    return 0;
}
