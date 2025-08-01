#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "Utils.h"
#include "FrequencyAnalyzer.h"
#include "CipherBreaker.h"

// Forward declarations for cipher breaker classes (will be implemented in later phases)
class CaesarBreaker;
class SubstitutionBreaker;
class VigenereBreaker;

/**
 * @brief Displays the main menu
 */
void displayMainMenu() {
    std::cout << "\n=== CryptoBreaker v1.0 ===" << std::endl;
    std::cout << "Frequency Analysis Cipher Breaking Tool" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Caesar Cipher Breaking" << std::endl;
    std::cout << "2. Substitution Cipher Breaking" << std::endl;
    std::cout << "3. Vigenère Cipher Breaking" << std::endl;
    std::cout << "4. Frequency Analysis Only" << std::endl;
    std::cout << "5. Batch File Processing" << std::endl;
    std::cout << "6. Help" << std::endl;
    std::cout << "7. Exit" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Enter your choice (1-7): ";
}

/**
 * @brief Gets user input for text analysis
 * @return User input text
 */
std::string getUserInput() {
    std::cout << "\nChoose input method:" << std::endl;
    std::cout << "1. Type text directly" << std::endl;
    std::cout << "2. Read from file" << std::endl;
    std::cout << "Enter choice (1-2): ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore(); // Clear newline from buffer
    
    std::string input;
    
    if (choice == 1) {
        std::cout << "Enter text to analyze: ";
        std::getline(std::cin, input);
    }
    else if (choice == 2) {
        std::cout << "Enter filename: ";
        std::string filename;
        std::getline(std::cin, filename);
        
        input = Utils::readFile(filename);
        if (input.empty()) {
            std::cout << "Error reading file. Please try again." << std::endl;
            return getUserInput();
        }
        std::cout << "File loaded successfully. (" << input.length() << " characters)" << std::endl;
    }
    else {
        std::cout << "Invalid choice. Please try again." << std::endl;
        return getUserInput();
    }
    
    return input;
}

/**
 * @brief Performs frequency analysis and displays results
 * @param text Text to analyze
 */
void performFrequencyAnalysis(const std::string& text) {
    FrequencyAnalyzer analyzer;
    
    std::cout << "\n=== Frequency Analysis Results ===" << std::endl;
    
    // Calculate frequencies
    auto frequencies = analyzer.calculateFrequency(text);
    
    // Display character frequencies
    std::cout << "\nCharacter Frequencies:" << std::endl;
    std::cout << "Char | Frequency | Percentage" << std::endl;
    std::cout << "-----+-----------+-----------" << std::endl;
    
    for (const auto& pair : frequencies) {
        std::cout << "  " << pair.first << "  |   " 
                  << std::fixed << std::setprecision(2) << pair.second 
                  << "%     | ";
        
        // Simple bar chart
        int barLength = static_cast<int>(pair.second / 2); // Scale down for display
        std::cout << std::string(barLength, '*') << std::endl;
    }
    
    // Language detection
    std::string detectedLang = analyzer.detectLanguage(text);
    std::cout << "\nDetected Language: " << detectedLang << std::endl;
    
    // Index of Coincidence
    double ic = analyzer.calculateIndexOfCoincidence(text);
    std::cout << "Index of Coincidence: " << std::fixed << std::setprecision(4) << ic << std::endl;
    
    // Interpretation
    if (ic > 0.06) {
        std::cout << "IC suggests: Monoalphabetic cipher (Caesar, Substitution)" << std::endl;
    } else if (ic > 0.04) {
        std::cout << "IC suggests: Polyalphabetic cipher (Vigenère)" << std::endl;
    } else {
        std::cout << "IC suggests: Random text or complex cipher" << std::endl;
    }
    
    // Common bigrams and trigrams
    auto bigrams = analyzer.findCommonBigrams(text, 5);
    std::cout << "\nMost Common Bigrams:" << std::endl;
    for (const auto& bg : bigrams) {
        std::cout << bg.first << " (" << std::fixed << std::setprecision(2) 
                  << bg.second << "%) ";
    }
    std::cout << std::endl;
    
    auto trigrams = analyzer.findCommonTrigrams(text, 5);
    std::cout << "\nMost Common Trigrams:" << std::endl;
    for (const auto& tg : trigrams) {
        std::cout << tg.first << " (" << std::fixed << std::setprecision(2) 
                  << tg.second << "%) ";
    }
    std::cout << std::endl;
}

/**
 * @brief Placeholder for Caesar cipher breaking (Phase 2)
 */
void handleCaesarBreaking() {
    std::cout << "\n=== Caesar Cipher Breaking ===" << std::endl;
    std::cout << "This feature will be implemented in Phase 2." << std::endl;
    std::cout << "For now, you can use frequency analysis to manually determine the shift." << std::endl;
    
    std::string input = getUserInput();
    if (!Utils::isValidInput(input)) {
        std::cout << "Invalid input. Please provide text with sufficient alphabetic characters." << std::endl;
        return;
    }
    
    performFrequencyAnalysis(input);
    
    // Simple manual Caesar demonstration
    std::cout << "\nManual Caesar Analysis:" << std::endl;
    std::cout << "Try different shift values (0-25) to see if meaningful text emerges." << std::endl;
}

/**
 * @brief Placeholder for Substitution cipher breaking (Phase 3)
 */
void handleSubstitutionBreaking() {
    std::cout << "\n=== Substitution Cipher Breaking ===" << std::endl;
    std::cout << "This feature will be implemented in Phase 3." << std::endl;
    
    std::string input = getUserInput();
    if (!Utils::isValidInput(input)) {
        std::cout << "Invalid input. Please provide text with sufficient alphabetic characters." << std::endl;
        return;
    }
    
    performFrequencyAnalysis(input);
}

/**
 * @brief Placeholder for Vigenère cipher breaking (Phase 4)
 */
void handleVigenereBreaking() {
    std::cout << "\n=== Vigenère Cipher Breaking ===" << std::endl;
    std::cout << "This feature will be implemented in Phase 4." << std::endl;
    
    std::string input = getUserInput();
    if (!Utils::isValidInput(input)) {
        std::cout << "Invalid input. Please provide text with sufficient alphabetic characters." << std::endl;
        return;
    }
    
    performFrequencyAnalysis(input);
}

/**
 * @brief Handles batch file processing
 */
void handleBatchProcessing() {
    std::cout << "\n=== Batch File Processing ===" << std::endl;
    std::cout << "Enter directory path containing text files: ";
    
    std::string dirPath;
    std::getline(std::cin, dirPath);
    
    std::cout << "Batch processing will be implemented in Phase 5." << std::endl;
}

/**
 * @brief Displays help information
 */
void displayHelp() {
    std::cout << "\n=== CryptoBreaker Help ===" << std::endl;
    std::cout << "\nSupported Cipher Types:" << std::endl;
    std::cout << "• Caesar Cipher: Simple shift cipher (ROT-N)" << std::endl;
    std::cout << "• Substitution Cipher: Each letter mapped to another" << std::endl;
    std::cout << "• Vigenère Cipher: Polyalphabetic cipher with keyword" << std::endl;
    
    std::cout << "\nFrequency Analysis:" << std::endl;
    std::cout << "• Analyzes character frequency distribution" << std::endl;
    std::cout << "• Detects language (English/Turkish)" << std::endl;
    std::cout << "• Calculates Index of Coincidence" << std::endl;
    std::cout << "• Finds common bigrams and trigrams" << std::endl;
    
    std::cout << "\nInput Methods:" << std::endl;
    std::cout << "• Direct text input" << std::endl;
    std::cout << "• File input (.txt files)" << std::endl;
    
    std::cout << "\nTips for Best Results:" << std::endl;
    std::cout << "• Use texts with at least 100 characters" << std::endl;
    std::cout << "• Ensure text contains mostly alphabetic characters" << std::endl;
    std::cout << "• Remove numbers and special characters if possible" << std::endl;
    
    std::cout << "\nProject Status:" << std::endl;
    std::cout << "✅ Phase 1: Basic infrastructure and frequency analysis" << std::endl;
    std::cout << "⏳ Phase 2: Caesar cipher breaking" << std::endl;
    std::cout << "⏳ Phase 3: Substitution cipher breaking" << std::endl;
    std::cout << "⏳ Phase 4: Vigenère cipher breaking" << std::endl;
    std::cout << "⏳ Phase 5: Advanced features and optimization" << std::endl;
}

/**
 * @brief Main application loop
 */
int main() {
    std::cout << "Welcome to CryptoBreaker!" << std::endl;
    std::cout << "Classical Cipher Analysis Tool" << std::endl;
    
    int choice;
    do {
        displayMainMenu();
        std::cin >> choice;
        std::cin.ignore(); // Clear newline from buffer
        
        switch (choice) {
            case 1:
                handleCaesarBreaking();
                break;
            case 2:
                handleSubstitutionBreaking();
                break;
            case 3:
                handleVigenereBreaking();
                break;
            case 4: {
                std::string input = getUserInput();
                if (Utils::isValidInput(input)) {
                    performFrequencyAnalysis(input);
                } else {
                    std::cout << "Invalid input. Please provide text with sufficient alphabetic characters." << std::endl;
                }
                break;
            }
            case 5:
                handleBatchProcessing();
                break;
            case 6:
                displayHelp();
                break;
            case 7:
                std::cout << "\nThank you for using CryptoBreaker!" << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please select 1-7." << std::endl;
                break;
        }
        
        if (choice != 7) {
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        }
        
    } while (choice != 7);
    
    return 0;
}

// Include guard for compilation
#include <iomanip>