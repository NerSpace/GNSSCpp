#include <iostream>
#include <vector>
#include <array>

std::vector<int> generateGoldCode(int PRN) {
    // Define the G2 shift values (as per the provided Python array)
    std::array<int, 32> g2s = {-1, 5, 6, 7, 8, 17, 18, 139, 140, 141, 251,
                               252, 254, 255, 256, 257, 258, 469, 470, 471, 472, 
                               473, 474, 509, 512, 513, 514, 515, 516, 859, 860,
                               861, 862, 145, 175};

    // Pick the right shift value for the given PRN number
    int g2shift = g2s[PRN];

    // Initialize the G1 and G2 codes
    std::vector<int> g1(1023, 0);
    std::vector<int> g2(1023, 0);
    
    // Initialize shift registers for G1 and G2
    std::array<int, 10> regG1 = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}; // G1 register
    std::array<int, 10> regG2 = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}; // G2 register

    // Generate G1 code based on feedback polynomial
    for (int i = 0; i < 1023; ++i) {
        g1[i] = regG1[9];  // Output G1 signal
        int saveBit = regG1[2] * regG1[9];  // Feedback bit
        // Shift the register
        for (int j = 9; j > 0; --j) {
            regG1[j] = regG1[j - 1];
        }
        regG1[0] = saveBit;  // Set the new value for regG1[0]
    }

    // Generate G2 code based on feedback polynomial
    for (int i = 0; i < 1023; ++i) {
        g2[i] = regG2[9];  // Output G2 signal
        int saveBit = regG2[1] * regG2[2] * regG2[5] * regG2[7] * regG2[8] * regG2[9];  // Feedback bit
        // Shift the register
        for (int j = 9; j > 0; --j) {
            regG2[j] = regG2[j - 1];
        }
        regG2[0] = saveBit;  // Set the new value for regG2[0]
    }

    // Shift the G2 code by the appropriate amount
    std::vector<int> g2_shifted(1023);
    for (int i = 0; i < 1023; ++i) {
        g2_shifted[i] = g2[(i + (1023 - g2shift)) % 1023];  // Circular shift
    }

    // Form the single sample C/A code by multiplying G1 and G2
    std::vector<int> CAcode(1023);
    for (int i = 0; i < 1023; ++i) {
        CAcode[i] = -(g1[i] * g2_shifted[i]);  // Negative multiplication of G1 and G2
    }

    // Return the Gold Code (C/A code)
    return CAcode;
}

int main() {
    int PRN = 19;  // Example PRN number
    std::vector<int> goldCode = generateGoldCode(PRN);

    // Output the first 10 values of the Gold Code for demonstration
    for (int i = 0; i < 10; ++i) {
        std::cout << goldCode[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
