/**
 * Simple Example - Basic C program for obfuscation testing
 * This demonstrates how the obfuscator protects simple algorithms
 */

#include <stdio.h>
#include <stdlib.h>

// Secret algorithm that we want to protect
int secret_algorithm(int input) {
    int magic = 0x1337;
    int result = input;
    
    // Complex calculation that we want to hide
    for (int i = 0; i < 5; i++) {
        result = (result * 3 + magic) ^ (magic >> i);
        magic = (magic << 1) | 1;
    }
    
    return result;
}

// License validation function
int validate_license(const char* key) {
    if (key == NULL) return 0;
    
    int sum = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        sum += key[i] * (i + 1);
    }
    
    // Secret validation logic
    return (sum % 1337 == 42);
}

int main(int argc, char* argv[]) {
    printf("Protected Application v1.0\n");
    
    // Check license
    const char* license_key = "SECRET-KEY-123";
    if (!validate_license(license_key)) {
        printf("Invalid license key!\n");
        return 1;
    }
    
    printf("License validated successfully.\n");
    
    // Process some data
    int data[] = {10, 20, 30, 40, 50};
    int results[5];
    
    for (int i = 0; i < 5; i++) {
        results[i] = secret_algorithm(data[i]);
        printf("Processing: %d -> %d\n", data[i], results[i]);
    }
    
    printf("Processing complete.\n");
    return 0;
}
