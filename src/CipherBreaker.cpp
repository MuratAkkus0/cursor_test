#include "CipherBreaker.h"
#include "Utils.h"
#include "CaesarBreaker.h"
#include "SubstitutionBreaker.h"
#include "VigenereBreaker.h"
#include <iostream>
#include <chrono>
#include <iomanip>

bool CipherBreaker::validateInput(const std::string& ciphertext) const {
    if (ciphertext.empty()) {
        if (verbose) {
            std::cerr << "Error: Empty input text" << std::endl;
        }
        return false;
    }
    
    if (!Utils::isValidInput(ciphertext)) {
        if (verbose) {
            std::cerr << "Error: Input must contain sufficient alphabetic characters" << std::endl;
        }
        return false;
    }
    
    return true;
}

void CipherBreaker::updateConfidence(double score, double maxScore) {
    if (maxScore > 0) {
        confidence = (score / maxScore) * 100.0;
        // Cap confidence at 100%
        if (confidence > 100.0) confidence = 100.0;
        if (confidence < 0.0) confidence = 0.0;
    } else {
        confidence = 0.0;
    }
}

void CipherBreaker::printVerbose(const std::string& message) const {
    if (verbose) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::cout << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S") << "] " 
                  << message << std::endl;
    }
}

// Factory function implementation
std::unique_ptr<CipherBreaker> createCipherBreaker(const std::string& cipherType) {
    if (cipherType == "caesar") {
        return std::make_unique<CaesarBreaker>();
    } else if (cipherType == "substitution") {
        return std::make_unique<SubstitutionBreaker>();
    } else if (cipherType == "vigenere") {
        return std::make_unique<VigenereBreaker>();
    }
    
    return nullptr;
}