#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "Utils.h"
#include "FrequencyAnalyzer.h"
#include "CipherBreaker.h"
#include "CaesarBreaker.h"
#include "SubstitutionBreaker.h"

// Forward declarations for cipher breaker classes (will be implemented in later phases)
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
 * @brief Handles Caesar cipher breaking with full automation
 */
void handleCaesarBreaking() {
    std::cout << "\n=== Caesar Cipher Breaking ===" << std::endl;
    std::cout << "Automatic Caesar cipher analysis using frequency analysis." << std::endl;
    
    std::string input = getUserInput();
    if (!Utils::isValidInput(input)) {
        std::cout << "Invalid input. Please provide text with sufficient alphabetic characters." << std::endl;
        return;
    }
    
    // Create Caesar breaker
    auto caesarBreaker = createCipherBreaker("caesar");
    if (!caesarBreaker) {
        std::cout << "Error: Could not create Caesar breaker instance." << std::endl;
        return;
    }
    
    // Enable verbose mode for detailed output
    std::cout << "\nWould you like verbose output? (y/n): ";
    char verbose;
    std::cin >> verbose;
    std::cin.ignore();
    
    if (verbose == 'y' || verbose == 'Y') {
        caesarBreaker->setVerbose(true);
    }
    
    // Perform analysis
    std::cout << "\nAnalyzing Caesar cipher..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    std::string result = caesarBreaker->breakCipher(input);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Display results
    std::cout << "\n=== Caesar Analysis Results ===" << std::endl;
    std::cout << "Analysis time: " << duration.count() << " ms" << std::endl;
    std::cout << "Confidence: " << std::fixed << std::setprecision(1) 
              << caesarBreaker->getConfidence() << "%" << std::endl;
    
    std::cout << "\nMost likely plaintext:" << std::endl;
    std::cout << "\"" << result << "\"" << std::endl;
    
    // Show alternative solutions
    std::cout << "\nTop 3 alternative solutions:" << std::endl;
    auto solutions = caesarBreaker->getPossibleSolutions(input);
    for (size_t i = 0; i < std::min(size_t(3), solutions.size()); i++) {
        std::string preview = solutions[i].substr(0, std::min(50, static_cast<int>(solutions[i].length())));
        std::cout << (i + 1) << ". \"" << preview;
        if (solutions[i].length() > 50) std::cout << "...";
        std::cout << "\"" << std::endl;
    }
    
    // Ask if user wants to save result
    std::cout << "\nSave result to file? (y/n): ";
    char save;
    std::cin >> save;
    std::cin.ignore();
    
    if (save == 'y' || save == 'Y') {
        std::cout << "Enter filename: ";
        std::string filename;
        std::getline(std::cin, filename);
        
        if (Utils::writeFile(filename, result)) {
            std::cout << "Result saved to " << filename << std::endl;
        } else {
            std::cout << "Error saving file." << std::endl;
        }
    }
}

/**
 * @brief Handles Substitution cipher breaking with advanced optimization
 */
void handleSubstitutionBreaking() {
    std::cout << "\n=== Substitution Cipher Breaking ===" << std::endl;
    std::cout << "Advanced substitution cipher analysis using frequency analysis and optimization." << std::endl;
    std::cout << "Warning: This process may take longer than Caesar cipher breaking." << std::endl;
    
    std::string input = getUserInput();
    if (!Utils::isValidInput(input)) {
        std::cout << "Invalid input. Please provide text with sufficient alphabetic characters." << std::endl;
        return;
    }
    
    // Create Substitution breaker
    auto substitutionBreaker = createCipherBreaker("substitution");
    if (!substitutionBreaker) {
        std::cout << "Error: Could not create Substitution breaker instance." << std::endl;
        return;
    }
    
    // Get optimization method preference
    std::cout << "\nSelect optimization method:" << std::endl;
    std::cout << "1. Frequency analysis only (fast)" << std::endl;
    std::cout << "2. Hill climbing optimization (medium)" << std::endl;
    std::cout << "3. Simulated annealing (slow, best quality)" << std::endl;
    std::cout << "4. Hybrid approach (recommended)" << std::endl;
    std::cout << "Enter choice (1-4): ";
    
    int methodChoice;
    std::cin >> methodChoice;
    std::cin.ignore();
    
    auto substBreaker = dynamic_cast<SubstitutionBreaker*>(substitutionBreaker.get());
    if (substBreaker) {
        switch (methodChoice) {
            case 1: substBreaker->setOptimizationMethod("frequency"); break;
            case 2: substBreaker->setOptimizationMethod("hill_climbing"); break;
            case 3: substBreaker->setOptimizationMethod("simulated_annealing"); break;
            case 4: 
            default: substBreaker->setOptimizationMethod("hybrid"); break;
        }
    }
    
    // Enable verbose mode option
    std::cout << "\nWould you like verbose output? (y/n): ";
    char verbose;
    std::cin >> verbose;
    std::cin.ignore();
    
    if (verbose == 'y' || verbose == 'Y') {
        substitutionBreaker->setVerbose(true);
    }
    
    // Perform analysis
    std::cout << "\nAnalyzing Substitution cipher..." << std::endl;
    std::cout << "This may take several seconds or minutes depending on text length and method..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::string result = substitutionBreaker->breakCipher(input);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Display results
    std::cout << "\n=== Substitution Analysis Results ===" << std::endl;
    std::cout << "Analysis time: " << duration.count() << " ms" << std::endl;
    std::cout << "Confidence: " << std::fixed << std::setprecision(1) 
              << substitutionBreaker->getConfidence() << "%" << std::endl;
    
    std::cout << "\nMost likely plaintext:" << std::endl;
    std::cout << "\"" << result << "\"" << std::endl;
    
    // Show alternative solutions
    std::cout << "\nAlternative solutions:" << std::endl;
    auto solutions = substitutionBreaker->getPossibleSolutions(input);
    for (size_t i = 0; i < std::min(size_t(3), solutions.size()); i++) {
        std::string preview = solutions[i].substr(0, std::min(60, static_cast<int>(solutions[i].length())));
        std::cout << (i + 1) << ". \"" << preview;
        if (solutions[i].length() > 60) std::cout << "...";
        std::cout << "\"" << std::endl;
    }
    
    // Show optimization history if available
    if (substBreaker && verbose == 'y') {
        auto history = substBreaker->getOptimizationHistory();
        if (!history.empty()) {
            std::cout << "\nOptimization History (last 5 improvements):" << std::endl;
            int count = 0;
            for (auto it = history.rbegin(); it != history.rend() && count < 5; ++it, ++count) {
                std::cout << "Iteration " << it->first << ": Score " 
                         << std::fixed << std::setprecision(4) << it->second << std::endl;
            }
        }
    }
    
    // Ask if user wants to save result
    std::cout << "\nSave result to file? (y/n): ";
    char save;
    std::cin >> save;
    std::cin.ignore();
    
    if (save == 'y' || save == 'Y') {
        std::cout << "Enter filename: ";
        std::string filename;
        std::getline(std::cin, filename);
        
        if (Utils::writeFile(filename, result)) {
            std::cout << "Result saved to " << filename << std::endl;
        } else {
            std::cout << "Error saving file." << std::endl;
        }
    }
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