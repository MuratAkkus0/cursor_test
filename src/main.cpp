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
#include "VigenereBreaker.h"
#include "AutoCipherDetector.h"

// All cipher breakers now implemented!

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
    std::cout << "4. Automatic Cipher Detection & Breaking" << std::endl;
    std::cout << "5. Frequency Analysis Only" << std::endl;
    std::cout << "6. Batch File Processing" << std::endl;
    std::cout << "7. Help" << std::endl;
    std::cout << "8. Exit" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Enter your choice (1-8): ";
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
 * @brief Handles Vigenère cipher breaking with advanced key detection
 */
void handleVigenereBreaking() {
    std::cout << "\n=== Vigenère Cipher Breaking ===" << std::endl;
    std::cout << "Advanced polyalphabetic cipher analysis using Kasiski examination and IC analysis." << std::endl;
    std::cout << "Note: Vigenère analysis requires longer texts for reliable results." << std::endl;
    
    std::string input = getUserInput();
    if (!Utils::isValidInput(input)) {
        std::cout << "Invalid input. Please provide text with sufficient alphabetic characters." << std::endl;
        return;
    }
    
    if (input.length() < 50) {
        std::cout << "\nWarning: Text is quite short (" << input.length() 
                  << " characters). Vigenère analysis works best with 100+ characters." << std::endl;
        std::cout << "Continue anyway? (y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.ignore();
        if (choice != 'y' && choice != 'Y') {
            return;
        }
    }
    
    // Create Vigenère breaker
    auto vigenereBreaker = createCipherBreaker("vigenere");
    if (!vigenereBreaker) {
        std::cout << "Error: Could not create Vigenère breaker instance." << std::endl;
        return;
    }
    
    // Configure key length range
    std::cout << "\nKey length configuration:" << std::endl;
    std::cout << "1. Use default range (2-20)" << std::endl;
    std::cout << "2. Specify custom range" << std::endl;
    std::cout << "Enter choice (1-2): ";
    
    int rangeChoice;
    std::cin >> rangeChoice;
    std::cin.ignore();
    
    auto vigBreaker = dynamic_cast<VigenereBreaker*>(vigenereBreaker.get());
    if (vigBreaker && rangeChoice == 2) {
        std::cout << "Enter minimum key length (2-10): ";
        int minLen;
        std::cin >> minLen;
        std::cout << "Enter maximum key length (5-20): ";
        int maxLen;
        std::cin >> maxLen;
        std::cin.ignore();
        
        vigBreaker->setKeyLengthRange(std::max(2, minLen), std::min(20, maxLen));
        std::cout << "Key length range set to " << std::max(2, minLen) 
                  << "-" << std::min(20, maxLen) << std::endl;
    }
    
    // Enable verbose mode option
    std::cout << "\nWould you like verbose output to see the analysis process? (y/n): ";
    char verbose;
    std::cin >> verbose;
    std::cin.ignore();
    
    if (verbose == 'y' || verbose == 'Y') {
        vigenereBreaker->setVerbose(true);
    }
    
    // Perform analysis
    std::cout << "\nAnalyzing Vigenère cipher..." << std::endl;
    std::cout << "This involves key length detection and Caesar analysis for each position..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::string result = vigenereBreaker->breakCipher(input);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Display results
    std::cout << "\n=== Vigenère Analysis Results ===" << std::endl;
    std::cout << "Analysis time: " << duration.count() << " ms" << std::endl;
    std::cout << "Confidence: " << std::fixed << std::setprecision(1) 
              << vigenereBreaker->getConfidence() << "%" << std::endl;
    
    if (!result.empty()) {
        std::cout << "\nMost likely plaintext:" << std::endl;
        std::cout << "\"" << result << "\"" << std::endl;
        
        // Show alternative solutions
        std::cout << "\nAlternative solutions:" << std::endl;
        auto solutions = vigenereBreaker->getPossibleSolutions(input);
        for (size_t i = 0; i < std::min(size_t(3), solutions.size()); i++) {
            std::string preview = solutions[i].substr(0, std::min(80, static_cast<int>(solutions[i].length())));
            std::cout << (i + 1) << ". \"" << preview;
            if (solutions[i].length() > 80) std::cout << "...";
            std::cout << "\"" << std::endl;
        }
        
        // Show analysis details if verbose
        if (vigBreaker && verbose == 'y') {
            auto analysisResults = vigBreaker->getAnalysisResults();
            
            std::cout << "\n=== Analysis Details ===" << std::endl;
            
            if (analysisResults.find("kasiski") != analysisResults.end()) {
                std::cout << "Kasiski Examination Results (top 3):" << std::endl;
                auto kasiskiResults = analysisResults["kasiski"];
                for (size_t i = 0; i < std::min(size_t(3), kasiskiResults.size()); i++) {
                    std::cout << "  Length " << kasiskiResults[i].first 
                              << ": Score " << std::fixed << std::setprecision(3) 
                              << kasiskiResults[i].second << std::endl;
                }
            }
            
            if (analysisResults.find("ic") != analysisResults.end()) {
                std::cout << "Index of Coincidence Results (top 3):" << std::endl;
                auto icResults = analysisResults["ic"];
                for (size_t i = 0; i < std::min(size_t(3), icResults.size()); i++) {
                    std::cout << "  Length " << icResults[i].first 
                              << ": IC " << std::fixed << std::setprecision(4) 
                              << icResults[i].second << std::endl;
                }
            }
        }
    } else {
        std::cout << "\nNo reliable solution found. Possible reasons:" << std::endl;
        std::cout << "- Text may be too short for reliable analysis" << std::endl;
        std::cout << "- Key length may be outside the search range" << std::endl;
        std::cout << "- Text may not be a Vigenère cipher" << std::endl;
        std::cout << "- Text may contain too much noise or non-English content" << std::endl;
    }
    
    // Ask if user wants to save result
    if (!result.empty()) {
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
}

/**
 * @brief Handles automatic cipher detection and breaking
 */
void handleAutomaticCipherDetection() {
    std::cout << "\n=== Automatic Cipher Detection & Breaking ===" << std::endl;
    std::cout << "Intelligent analysis to detect cipher type and automatically break it." << std::endl;
    
    std::string input = getUserInput();
    if (!Utils::isValidInput(input)) {
        std::cout << "Invalid input. Please provide text with sufficient alphabetic characters." << std::endl;
        return;
    }
    
    // Create auto detector
    AutoCipherDetector detector;
    
    std::cout << "\nAnalyzing cipher type..." << std::endl;
    auto detection = detector.detectCipherType(input);
    
    // Display detection results
    std::cout << "\n=== Cipher Detection Results ===" << std::endl;
    std::cout << "Detected Type: " << detection.cipherType << std::endl;
    std::cout << "Confidence: " << std::fixed << std::setprecision(1) 
              << (detection.confidence * 100) << "%" << std::endl;
    std::cout << "Reasoning: " << detection.reasoning << std::endl;
    
    // Show all scores
    std::cout << "\nAll Cipher Type Scores:" << std::endl;
    for (const auto& score : detection.scores) {
        std::cout << "  " << score.first << ": " 
                  << std::fixed << std::setprecision(3) << score.second << std::endl;
    }
    
    // If we detected a cipher type, offer to break it
    if (detection.cipherType != "unknown" && detection.cipherType != "plaintext" && detection.confidence > 0.3) {
        std::cout << "\nWould you like to attempt breaking this " << detection.cipherType 
                  << " cipher? (y/n): ";
        char breakChoice;
        std::cin >> breakChoice;
        std::cin.ignore();
        
        if (breakChoice == 'y' || breakChoice == 'Y') {
            std::cout << "\nAttempting to break " << detection.cipherType << " cipher..." << std::endl;
            
            // Create appropriate cipher breaker
            auto cipherBreaker = createCipherBreaker(detection.cipherType);
            if (cipherBreaker) {
                // Enable verbose for auto-detected ciphers
                cipherBreaker->setVerbose(true);
                
                auto start = std::chrono::high_resolution_clock::now();
                std::string result = cipherBreaker->breakCipher(input);
                auto end = std::chrono::high_resolution_clock::now();
                
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                std::cout << "\n=== Cipher Breaking Results ===" << std::endl;
                std::cout << "Analysis time: " << duration.count() << " ms" << std::endl;
                std::cout << "Breaking confidence: " << std::fixed << std::setprecision(1) 
                          << cipherBreaker->getConfidence() << "%" << std::endl;
                
                if (!result.empty()) {
                    std::cout << "\nDecrypted text:" << std::endl;
                    std::cout << "\"" << result << "\"" << std::endl;
                    
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
                } else {
                    std::cout << "\nCould not successfully break the cipher." << std::endl;
                    std::cout << "You may want to try the specific cipher breaking methods manually." << std::endl;
                }
            } else {
                std::cout << "Error: Could not create cipher breaker for " << detection.cipherType << std::endl;
            }
        }
    } else if (detection.cipherType == "plaintext") {
        std::cout << "\nThe text appears to already be in plaintext (not encrypted)." << std::endl;
    } else {
        std::cout << "\nCould not reliably detect cipher type. You may want to:" << std::endl;
        std::cout << "- Try the specific cipher breaking methods manually" << std::endl;
        std::cout << "- Ensure the text is long enough for analysis (50+ characters recommended)" << std::endl;
        std::cout << "- Check that the text contains mostly alphabetic characters" << std::endl;
    }
}

/**
 * @brief Handles batch file processing
 */
void handleBatchProcessing() {
    std::cout << "\n=== Batch File Processing ===" << std::endl;
    std::cout << "Process multiple encrypted files automatically." << std::endl;
    
    std::cout << "\nEnter directory path containing text files: ";
    std::string dirPath;
    std::getline(std::cin, dirPath);
    
    // Check if directory exists
    if (!Utils::fileExists(dirPath)) {
        std::cout << "Error: Directory not found: " << dirPath << std::endl;
        return;
    }
    
    std::cout << "\nSelect processing mode:" << std::endl;
    std::cout << "1. Automatic detection and breaking" << std::endl;
    std::cout << "2. Caesar cipher only" << std::endl;
    std::cout << "3. Substitution cipher only" << std::endl;
    std::cout << "4. Vigenère cipher only" << std::endl;
    std::cout << "Enter choice (1-4): ";
    
    int modeChoice;
    std::cin >> modeChoice;
    std::cin.ignore();
    
    std::cout << "\nCreate detailed CSV report? (y/n): ";
    char createReport;
    std::cin >> createReport;
    std::cin.ignore();
    
    // Process files
    std::vector<std::string> results;
    std::vector<std::string> filenames;
    int processed = 0, successful = 0;
    
    std::cout << "\n=== Processing Files ===" << std::endl;
    
    // For demo purposes, we'll simulate processing
    // In a real implementation, you'd use filesystem APIs to list directory contents
    std::cout << "Note: Full directory scanning requires filesystem library." << std::endl;
    std::cout << "For now, enter filenames manually (empty line to finish):" << std::endl;
    
    std::string filename;
    while (true) {
        std::cout << "Enter filename (or press Enter to finish): ";
        std::getline(std::cin, filename);
        if (filename.empty()) break;
        
        std::string fullPath = dirPath + "/" + filename;
        std::string content = Utils::readFile(fullPath);
        
        if (content.empty()) {
            std::cout << "  ❌ Error reading: " << filename << std::endl;
            continue;
        }
        
        processed++;
        filenames.push_back(filename);
        
        std::cout << "  📄 Processing: " << filename << " (" << content.length() << " chars)";
        
        std::string result;
        if (modeChoice == 1) {
            // Automatic detection
            AutoCipherDetector detector;
            auto detection = detector.detectCipherType(content);
            
            if (detection.confidence > 0.3) {
                auto breaker = createCipherBreaker(detection.cipherType);
                if (breaker) {
                    result = breaker->breakCipher(content);
                    if (!result.empty()) {
                        std::cout << " ✅ [" << detection.cipherType << "]" << std::endl;
                        successful++;
                    } else {
                        std::cout << " ⚠️  [" << detection.cipherType << " - failed]" << std::endl;
                        result = "[FAILED TO DECRYPT]";
                    }
                } else {
                    std::cout << " ❌ [Unknown cipher]" << std::endl;
                    result = "[UNKNOWN CIPHER TYPE]";
                }
            } else {
                std::cout << " ❌ [Detection failed]" << std::endl;
                result = "[DETECTION FAILED]";
            }
        } else {
            // Specific cipher type
            std::string cipherType;
            switch (modeChoice) {
                case 2: cipherType = "caesar"; break;
                case 3: cipherType = "substitution"; break;
                case 4: cipherType = "vigenere"; break;
            }
            
            auto breaker = createCipherBreaker(cipherType);
            if (breaker) {
                result = breaker->breakCipher(content);
                if (!result.empty()) {
                    std::cout << " ✅ [" << cipherType << "]" << std::endl;
                    successful++;
                } else {
                    std::cout << " ❌ [" << cipherType << " - failed]" << std::endl;
                    result = "[FAILED TO DECRYPT]";
                }
            }
        }
        
        results.push_back(result);
    }
    
    // Summary
    std::cout << "\n=== Batch Processing Summary ===" << std::endl;
    std::cout << "Files processed: " << processed << std::endl;
    std::cout << "Successfully decrypted: " << successful << std::endl;
    std::cout << "Success rate: " << (processed > 0 ? (successful * 100 / processed) : 0) << "%" << std::endl;
    
    // Save results
    if (processed > 0) {
        std::cout << "\nSave results to files? (y/n): ";
        char saveResults;
        std::cin >> saveResults;
        std::cin.ignore();
        
        if (saveResults == 'y' || saveResults == 'Y') {
            for (size_t i = 0; i < results.size(); i++) {
                std::string outputFile = "decrypted_" + filenames[i];
                if (Utils::writeFile(outputFile, results[i])) {
                    std::cout << "  ✅ Saved: " << outputFile << std::endl;
                }
            }
        }
        
        // Create CSV report if requested
        if (createReport == 'y' || createReport == 'Y') {
            std::string csvContent = "Filename,Status,Length,Result_Preview\n";
            for (size_t i = 0; i < results.size(); i++) {
                std::string status = (results[i].find("[FAILED") != std::string::npos || 
                                     results[i].find("[UNKNOWN") != std::string::npos) ? "Failed" : "Success";
                std::string preview = results[i].substr(0, std::min(50, (int)results[i].length()));
                // Escape commas in preview
                for (auto& c : preview) if (c == ',') c = ';';
                csvContent += filenames[i] + "," + status + "," + std::to_string(results[i].length()) + 
                             ",\"" + preview + "\"\n";
            }
            
            if (Utils::writeFile("batch_report.csv", csvContent)) {
                std::cout << "  📊 CSV report saved: batch_report.csv" << std::endl;
            }
        }
    }
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
    std::cout << "✅ Phase 2: Caesar cipher breaking" << std::endl;
    std::cout << "✅ Phase 3: Substitution cipher breaking" << std::endl;
    std::cout << "✅ Phase 4: Vigenère cipher breaking" << std::endl;
    std::cout << "✅ Phase 5: Advanced features (auto-detection, batch processing)" << std::endl;
    std::cout << "\nCompletion: ~90% (Ready for production use!)" << std::endl;
}

/**
 * @brief Shows command line usage
 */
void showUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -t, --type <cipher>     Cipher type (caesar|substitution|vigenere|auto)" << std::endl;
    std::cout << "  -i, --input <text>      Input text to decrypt" << std::endl;
    std::cout << "  -f, --file <filename>   Input file to process" << std::endl;
    std::cout << "  -o, --output <filename> Output file for results" << std::endl;
    std::cout << "  -v, --verbose           Enable verbose output" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << programName << " -t caesar -i \"KHOOR ZRUOG\"" << std::endl;
    std::cout << "  " << programName << " -t auto -f encrypted.txt -o result.txt -v" << std::endl;
}

/**
 * @brief Processes command line arguments
 */
int processCommandLine(int argc, char* argv[]) {
    std::string cipherType = "";
    std::string inputText = "";
    std::string inputFile = "";
    std::string outputFile = "";
    bool verbose = false;
    
    // Simple argument parsing
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            showUsage(argv[0]);
            return 0;
        }
        else if ((arg == "-t" || arg == "--type") && i + 1 < argc) {
            cipherType = argv[++i];
        }
        else if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            inputText = argv[++i];
        }
        else if ((arg == "-f" || arg == "--file") && i + 1 < argc) {
            inputFile = argv[++i];
        }
        else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            outputFile = argv[++i];
        }
        else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
        else {
            std::cout << "Unknown argument: " << arg << std::endl;
            showUsage(argv[0]);
            return 1;
        }
    }
    
    // Validate arguments
    if (cipherType.empty()) {
        std::cout << "Error: Cipher type is required (-t or --type)" << std::endl;
        return 1;
    }
    
    std::string input;
    if (!inputFile.empty()) {
        input = Utils::readFile(inputFile);
        if (input.empty()) {
            std::cout << "Error: Could not read file: " << inputFile << std::endl;
            return 1;
        }
    } else if (!inputText.empty()) {
        input = inputText;
    } else {
        std::cout << "Error: Input text or file is required (-i or -f)" << std::endl;
        return 1;
    }
    
    if (!Utils::isValidInput(input)) {
        std::cout << "Error: Input text must contain sufficient alphabetic characters" << std::endl;
        return 1;
    }
    
    // Process the cipher
    std::cout << "CryptoBreaker v1.0 - Command Line Mode" << std::endl;
    std::cout << "Cipher Type: " << cipherType << std::endl;
    std::cout << "Input Length: " << input.length() << " characters" << std::endl;
    
    std::string result;
    auto start = std::chrono::high_resolution_clock::now();
    
    if (cipherType == "auto") {
        AutoCipherDetector detector;
        auto detection = detector.detectCipherType(input);
        
        std::cout << "Detected Type: " << detection.cipherType;
        std::cout << " (" << std::fixed << std::setprecision(1) << (detection.confidence * 100) << "% confidence)" << std::endl;
        
        if (detection.confidence > 0.3) {
            auto breaker = createCipherBreaker(detection.cipherType);
            if (breaker) {
                breaker->setVerbose(verbose);
                result = breaker->breakCipher(input);
            }
        }
    } else {
        auto breaker = createCipherBreaker(cipherType);
        if (!breaker) {
            std::cout << "Error: Unknown cipher type: " << cipherType << std::endl;
            return 1;
        }
        
        breaker->setVerbose(verbose);
        result = breaker->breakCipher(input);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Output results
    if (!result.empty()) {
        std::cout << "\n=== Results ===" << std::endl;
        std::cout << "Analysis Time: " << duration.count() << " ms" << std::endl;
        std::cout << "Decrypted Text: \"" << result << "\"" << std::endl;
        
        // Save to file if specified
        if (!outputFile.empty()) {
            if (Utils::writeFile(outputFile, result)) {
                std::cout << "Result saved to: " << outputFile << std::endl;
            } else {
                std::cout << "Error: Could not save to file: " << outputFile << std::endl;
                return 1;
            }
        }
    } else {
        std::cout << "\nError: Could not decrypt the text" << std::endl;
        return 1;
    }
    
    return 0;
}

/**
 * @brief Main application entry point
 */
int main(int argc, char* argv[]) {
    // Check for command line arguments
    if (argc > 1) {
        return processCommandLine(argc, argv);
    }
    
    // Interactive mode
    std::cout << "Welcome to CryptoBreaker!" << std::endl;
    std::cout << "Classical Cipher Analysis Tool" << std::endl;
    std::cout << "(Use --help for command line options)" << std::endl;
    
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
            case 4:
                handleAutomaticCipherDetection();
                break;
            case 5: {
                std::string input = getUserInput();
                if (Utils::isValidInput(input)) {
                    performFrequencyAnalysis(input);
                } else {
                    std::cout << "Invalid input. Please provide text with sufficient alphabetic characters." << std::endl;
                }
                break;
            }
            case 6:
                handleBatchProcessing();
                break;
            case 7:
                displayHelp();
                break;
            case 8:
                std::cout << "\nThank you for using CryptoBreaker!" << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please select 1-8." << std::endl;
                break;
        }
        
        if (choice != 8) {
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        }
        
    } while (choice != 8);
    
    return 0;
}