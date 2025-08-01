#include "VigenereBreaker.h"
#include "Utils.h"
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <set>
#include <thread>
#include <future>
#include <mutex>

VigenereBreaker::VigenereBreaker() 
    : minKeyLength(2), maxKeyLength(20), minSubstringLength(3) {
    // Initialize with reasonable defaults
}

std::string VigenereBreaker::breakCipher(const std::string& ciphertext) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!validateInput(ciphertext)) {
        return "";
    }
    
    printVerbose("Starting Vigenère cipher analysis...");
    printVerbose("Text length: " + std::to_string(ciphertext.length()));
    printVerbose("Key length range: " + std::to_string(minKeyLength) + 
                " to " + std::to_string(maxKeyLength));
    
    analysisResults.clear();
    
    // Step 1: Find possible key lengths
    std::vector<int> candidateKeyLengths = findKeyLength(ciphertext);
    
    if (candidateKeyLengths.empty()) {
        printVerbose("No viable key lengths found");
        return "";
    }
    
    printVerbose("Found " + std::to_string(candidateKeyLengths.size()) + " candidate key lengths");
    
    // Step 2: Try each key length in parallel and find the best result
    std::string bestResult = "";
    double bestScore = 0.0;
    std::string bestKey = "";
    
    if (candidateKeyLengths.size() == 1) {
        // Single key length - no need for threading
        int keyLength = candidateKeyLengths[0];
        printVerbose("Testing key length: " + std::to_string(keyLength));
        
        std::string key = findKey(ciphertext, keyLength);
        std::string decrypted = decrypt(ciphertext, key);
        double score = scorePlaintext(decrypted);
        
        printVerbose("Key: \"" + key + "\", Score: " + std::to_string(score));
        
        bestScore = score;
        bestResult = decrypted;
        bestKey = key;
    } else {
        // Multiple key lengths - use parallel processing
        const int numThreads = std::min(static_cast<int>(candidateKeyLengths.size()), 
                                        static_cast<int>(std::thread::hardware_concurrency()));
        
        printVerbose("Testing " + std::to_string(candidateKeyLengths.size()) + 
                    " key lengths using " + std::to_string(numThreads) + " threads");
        
        std::vector<std::future<std::tuple<std::string, std::string, double, int>>> futures;
        
        // Launch parallel tasks for key length testing
        for (int keyLength : candidateKeyLengths) {
            auto future = std::async(std::launch::async, [this, ciphertext, keyLength]() {
                std::string key = findKey(ciphertext, keyLength);
                std::string decrypted = decrypt(ciphertext, key);
                double score = scorePlaintext(decrypted);
                return std::make_tuple(key, decrypted, score, keyLength);
            });
            
            futures.push_back(std::move(future));
        }
        
        // Collect results and find the best one
        for (auto& future : futures) {
            auto [key, decrypted, score, keyLength] = future.get();
            
            printVerbose("Key length " + std::to_string(keyLength) + 
                        ": Key=\"" + key + "\", Score=" + std::to_string(score));
            
            if (score > bestScore) {
                bestScore = score;
                bestResult = decrypted;
                bestKey = key;
            }
        }
        
        printVerbose("Parallel key length analysis completed");
    }
    
    // Update confidence based on best score
    updateConfidence(bestScore, 1.0);
    
    // Calculate analysis time
    auto end = std::chrono::high_resolution_clock::now();
    analysisTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    
    printVerbose("Analysis completed in " + std::to_string(analysisTimeMs) + " ms");
    printVerbose("Best key found: \"" + bestKey + "\"");
    printVerbose("Confidence: " + std::to_string(confidence) + "%");
    
    return bestResult;
}

std::vector<std::string> VigenereBreaker::getPossibleSolutions(const std::string& ciphertext) {
    if (!validateInput(ciphertext)) {
        return {};
    }
    
    std::vector<std::string> solutions;
    std::vector<int> candidateKeyLengths = findKeyLength(ciphertext);
    
    // Try top 5 key lengths
    int maxTries = std::min(5, static_cast<int>(candidateKeyLengths.size()));
    
    for (int i = 0; i < maxTries; i++) {
        int keyLength = candidateKeyLengths[i];
        std::string key = findKey(ciphertext, keyLength);
        std::string solution = decrypt(ciphertext, key);
        solutions.push_back(solution);
    }
    
    return solutions;
}

double VigenereBreaker::scorePlaintext(const std::string& plaintext) {
    return analyzer.scoreEnglishness(plaintext);
}

std::vector<int> VigenereBreaker::findKeyLength(const std::string& ciphertext) {
    printVerbose("Finding key length using multiple methods...");
    
    std::string normalizedText = Utils::normalizeText(ciphertext);
    
    // Method 1: Kasiski Examination
    auto kasiskiResults = kasiskiExamination(normalizedText);
    analysisResults["kasiski"] = kasiskiResults;
    
    // Method 2: Index of Coincidence
    auto icResults = indexOfCoincidenceMethod(normalizedText);
    analysisResults["ic"] = icResults;
    
    // Combine results
    std::vector<int> combinedResults = combineKeyLengthResults(kasiskiResults, icResults);
    
    printVerbose("Key length analysis complete. Top candidates:");
    for (size_t i = 0; i < std::min(size_t(5), combinedResults.size()); i++) {
        printVerbose("  " + std::to_string(i + 1) + ". Length " + std::to_string(combinedResults[i]));
    }
    
    return combinedResults;
}

std::vector<std::pair<int, double>> VigenereBreaker::kasiskiExamination(const std::string& ciphertext) {
    printVerbose("Performing Kasiski examination...");
    
    // Find repeating substrings
    auto repeatingPatterns = findRepeatingSubstrings(ciphertext, minSubstringLength);
    
    // Collect all distances
    std::vector<int> allDistances;
    int significantPatterns = 0;
    
    for (const auto& pattern : repeatingPatterns) {
        if (pattern.second.size() >= 2) { // At least 2 occurrences
            for (size_t i = 1; i < pattern.second.size(); i++) {
                int distance = pattern.second[i] - pattern.second[0];
                if (distance >= minKeyLength) {
                    allDistances.push_back(distance);
                }
            }
            significantPatterns++;
        }
    }
    
    printVerbose("Found " + std::to_string(significantPatterns) + " significant repeating patterns");
    printVerbose("Collected " + std::to_string(allDistances.size()) + " distances");
    
    if (allDistances.empty()) {
        printVerbose("No significant distances found in Kasiski examination");
        return {};
    }
    
    // Analyze distance patterns
    auto keyLengthScores = analyzeDistancePatterns(allDistances);
    
    // Convert to vector and sort by score
    std::vector<std::pair<int, double>> results;
    for (const auto& pair : keyLengthScores) {
        if (pair.first >= minKeyLength && pair.first <= maxKeyLength) {
            results.emplace_back(pair.first, pair.second);
        }
    }
    
    std::sort(results.begin(), results.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    printVerbose("Kasiski examination found " + std::to_string(results.size()) + " candidates");
    
    return results;
}

std::vector<std::pair<int, double>> VigenereBreaker::indexOfCoincidenceMethod(const std::string& ciphertext) {
    printVerbose("Performing Index of Coincidence analysis...");
    
    std::vector<std::pair<int, double>> results;
    
    for (int keyLength = minKeyLength; keyLength <= maxKeyLength; keyLength++) {
        double ic = calculateICForKeyLength(ciphertext, keyLength);
        results.emplace_back(keyLength, ic);
        
        updateKeySearchProgress(keyLength, maxKeyLength, "IC", ic);
    }
    
    // Sort by IC value (higher is better for English)
    std::sort(results.begin(), results.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    printVerbose("IC analysis completed. Best IC values:");
    for (size_t i = 0; i < std::min(size_t(5), results.size()); i++) {
        printVerbose("  Length " + std::to_string(results[i].first) + 
                    ": IC = " + std::to_string(results[i].second));
    }
    
    return results;
}

double VigenereBreaker::calculateICForKeyLength(const std::string& text, int keyLength) {
    // Split text by key positions
    auto substrings = splitByKeyPosition(text, keyLength);
    
    // Calculate average IC for all substrings
    double totalIC = 0.0;
    int validSubstrings = 0;
    
    for (const std::string& substring : substrings) {
        if (substring.length() >= 2) { // Need at least 2 characters for IC
            double ic = analyzer.calculateIndexOfCoincidence(substring);
            totalIC += ic;
            validSubstrings++;
        }
    }
    
    return validSubstrings > 0 ? totalIC / validSubstrings : 0.0;
}

std::vector<std::string> VigenereBreaker::splitByKeyPosition(const std::string& text, int keyLength) {
    std::vector<std::string> substrings(keyLength);
    
    for (size_t i = 0; i < text.length(); i++) {
        int position = i % keyLength;
        substrings[position] += text[i];
    }
    
    return substrings;
}

std::map<std::string, std::vector<int>> VigenereBreaker::findRepeatingSubstrings(const std::string& text, int minLength) {
    std::map<std::string, std::vector<int>> patterns;
    
    // Find all substrings of specified minimum length
    for (size_t i = 0; i <= text.length() - minLength; i++) {
        for (int len = minLength; len <= std::min(static_cast<int>(text.length() - i), 10); len++) {
            std::string substring = text.substr(i, len);
            patterns[substring].push_back(i);
        }
    }
    
    // Keep only patterns that repeat
    std::map<std::string, std::vector<int>> repeatingPatterns;
    for (const auto& pattern : patterns) {
        if (pattern.second.size() >= 2) {
            repeatingPatterns[pattern.first] = pattern.second;
        }
    }
    
    return repeatingPatterns;
}

std::map<int, double> VigenereBreaker::analyzeDistancePatterns(const std::vector<int>& distances) {
    std::map<int, int> factorCounts;
    
    // For each distance, find its factors
    for (int distance : distances) {
        for (int factor = minKeyLength; factor <= maxKeyLength && factor <= distance; factor++) {
            if (distance % factor == 0) {
                factorCounts[factor]++;
            }
        }
    }
    
    // Convert counts to scores
    std::map<int, double> scores;
    int maxCount = 0;
    for (const auto& pair : factorCounts) {
        maxCount = std::max(maxCount, pair.second);
    }
    
    if (maxCount > 0) {
        for (const auto& pair : factorCounts) {
            scores[pair.first] = static_cast<double>(pair.second) / maxCount;
        }
    }
    
    return scores;
}

int VigenereBreaker::gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int VigenereBreaker::calculateGCD(const std::vector<int>& numbers) {
    if (numbers.empty()) return 1;
    if (numbers.size() == 1) return numbers[0];
    
    int result = numbers[0];
    for (size_t i = 1; i < numbers.size(); i++) {
        result = gcd(result, numbers[i]);
        if (result == 1) break; // Early termination
    }
    
    return result;
}

std::vector<int> VigenereBreaker::combineKeyLengthResults(const std::vector<std::pair<int, double>>& kasiskiResults,
                                                         const std::vector<std::pair<int, double>>& icResults) {
    std::map<int, double> combinedScores;
    
    // Add Kasiski scores (weight: 0.6)
    for (const auto& result : kasiskiResults) {
        combinedScores[result.first] += result.second * 0.6;
    }
    
    // Add IC scores (weight: 0.4)
    for (const auto& result : icResults) {
        combinedScores[result.first] += result.second * 0.4;
    }
    
    // Convert to sorted vector
    std::vector<std::pair<int, double>> sortedResults;
    for (const auto& pair : combinedScores) {
        sortedResults.emplace_back(pair.first, pair.second);
    }
    
    std::sort(sortedResults.begin(), sortedResults.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Extract just the key lengths
    std::vector<int> keyLengths;
    for (const auto& result : sortedResults) {
        keyLengths.push_back(result.first);
    }
    
    return keyLengths;
}

void VigenereBreaker::updateKeySearchProgress(int currentLength, int maxLength, 
                                             const std::string& method, double score) {
    if (verbose) {
        printVerbose(method + " analysis - Length " + std::to_string(currentLength) + 
                    "/" + std::to_string(maxLength) + ": " + std::to_string(score));
    }
}

std::string VigenereBreaker::findKey(const std::string& ciphertext, int keyLength) {
    printVerbose("Finding key for length " + std::to_string(keyLength) + "...");
    
    std::string normalizedText = Utils::normalizeText(ciphertext);
    
    // Split text by key positions
    auto substrings = splitByKeyPosition(normalizedText, keyLength);
    
    std::string key = "";
    key.reserve(keyLength);
    
    // Find key character for each position
    for (int position = 0; position < keyLength; position++) {
        if (position < static_cast<int>(substrings.size()) && !substrings[position].empty()) {
            char keyChar = findSubkey(substrings[position]);
            key += keyChar;
            
            if (verbose) {
                printVerbose("Position " + std::to_string(position) + ": '" + 
                           std::string(1, keyChar) + "' (shift " + 
                           std::to_string(charToShift(keyChar)) + ")");
            }
        } else {
            key += 'A'; // Default if no data
        }
    }
    
    printVerbose("Constructed key: \"" + key + "\"");
    
    return key;
}

char VigenereBreaker::findSubkey(const std::string& subtextCipher) {
    if (subtextCipher.empty()) {
        return 'A'; // Default
    }
    
    // Use Caesar breaker to find the best shift for this substring
    int bestShift = caesarBreaker.findBestKey(subtextCipher);
    
    // Convert shift to key character (shift 0 = 'A', shift 1 = 'B', etc.)
    return shiftToChar(bestShift);
}

std::string VigenereBreaker::encrypt(const std::string& plaintext, const std::string& key) {
    if (key.empty()) return plaintext;
    
    std::string result;
    result.reserve(plaintext.length());
    
    for (size_t i = 0; i < plaintext.length(); i++) {
        char c = plaintext[i];
        if (std::isalpha(c)) {
            char keyChar = key[i % key.length()];
            int shift = charToShift(std::toupper(keyChar));
            result += vigenereShift(c, shift);
        } else {
            result += c;
        }
    }
    
    return result;
}

std::string VigenereBreaker::decrypt(const std::string& ciphertext, const std::string& key) {
    if (key.empty()) return ciphertext;
    
    std::string result;
    result.reserve(ciphertext.length());
    
    for (size_t i = 0; i < ciphertext.length(); i++) {
        char c = ciphertext[i];
        if (std::isalpha(c)) {
            char keyChar = key[i % key.length()];
            int shift = charToShift(std::toupper(keyChar));
            // For decryption, we subtract the shift
            result += vigenereShift(c, 26 - shift);
        } else {
            result += c;
        }
    }
    
    return result;
}

char VigenereBreaker::vigenereShift(char c, int shift) {
    if (!std::isalpha(c)) {
        return c;
    }
    
    char base = std::isupper(c) ? 'A' : 'a';
    return static_cast<char>((c - base + shift) % 26 + base);
}

int VigenereBreaker::charToShift(char c) {
    if (std::isalpha(c)) {
        return std::toupper(c) - 'A';
    }
    return 0;
}

char VigenereBreaker::shiftToChar(int shift) {
    return static_cast<char>('A' + (shift % 26));
}

double VigenereBreaker::validateKey(const std::string& key, const std::string& ciphertext) {
    // Decrypt with the proposed key
    std::string decrypted = decrypt(ciphertext, key);
    
    // Score the result
    double score = scorePlaintext(decrypted);
    
    // Additional validation: check if key contains reasonable characters
    double keyValidityScore = 1.0;
    for (char c : key) {
        if (!std::isalpha(c)) {
            keyValidityScore *= 0.5; // Penalize non-alphabetic characters
        }
    }
    
    return score * keyValidityScore;
}