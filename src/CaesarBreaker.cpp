#include "CaesarBreaker.h"
#include "Utils.h"
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cmath>

CaesarBreaker::CaesarBreaker() : minTextLength(20) {
    // Initialize with default settings
}

std::string CaesarBreaker::breakCipher(const std::string& ciphertext) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!validateInput(ciphertext)) {
        return "";
    }
    
    printVerbose("Starting Caesar cipher analysis...");
    printVerbose("Text length: " + std::to_string(ciphertext.length()));
    
    // Find the best key
    int bestKey = findBestKey(ciphertext);
    
    // Decrypt with best key
    std::string result = decrypt(ciphertext, bestKey);
    
    // Calculate analysis time
    auto end = std::chrono::high_resolution_clock::now();
    analysisTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    
    printVerbose("Analysis completed in " + std::to_string(analysisTimeMs) + " ms");
    printVerbose("Best key found: " + std::to_string(bestKey));
    printVerbose("Confidence: " + std::to_string(confidence) + "%");
    
    return result;
}

std::vector<std::string> CaesarBreaker::getPossibleSolutions(const std::string& ciphertext) {
    if (!validateInput(ciphertext)) {
        return {};
    }
    
    auto keyScores = findPossibleKeys(ciphertext);
    std::vector<std::string> solutions;
    
    // Return top 5 solutions
    int maxSolutions = std::min(5, static_cast<int>(keyScores.size()));
    solutions.reserve(maxSolutions);
    
    for (int i = 0; i < maxSolutions; i++) {
        std::string solution = decrypt(ciphertext, keyScores[i].first);
        solutions.push_back(solution);
    }
    
    return solutions;
}

double CaesarBreaker::scorePlaintext(const std::string& plaintext) {
    return scoreText(plaintext);
}

std::vector<std::pair<int, double>> CaesarBreaker::findPossibleKeys(const std::string& ciphertext) {
    std::string normalizedText = Utils::normalizeText(ciphertext);
    std::vector<std::pair<int, double>> keyScores;
    keyScores.reserve(26);
    
    printVerbose("Testing all 26 possible Caesar keys...");
    
    // Test all possible keys (0-25)
    for (int key = 0; key < 26; key++) {
        std::string decrypted = caesarShift(normalizedText, key);
        double score = scoreText(decrypted);
        
        keyScores.emplace_back(key, score);
        lastAnalysis[key] = score;
        
        // Show preview for verbose mode
        if (verbose) {
            std::string preview = decrypted.substr(0, std::min(30, static_cast<int>(decrypted.length())));
            updateProgress(key, score, preview);
        }
    }
    
    // Sort by score (highest first)
    std::sort(keyScores.begin(), keyScores.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Update confidence based on best vs second best
    if (keyScores.size() >= 2) {
        double bestScore = keyScores[0].second;
        double secondBest = keyScores[1].second;
        double scoreDiff = bestScore - secondBest;
        confidence = std::min(100.0, (scoreDiff * 10.0) + 50.0); // Scale to 0-100%
    }
    
    return keyScores;
}

std::string CaesarBreaker::decrypt(const std::string& ciphertext, int key) {
    return caesarShift(ciphertext, key);
}

int CaesarBreaker::findBestKey(const std::string& ciphertext) {
    auto keyScores = findPossibleKeys(ciphertext);
    if (keyScores.empty()) {
        return 0;
    }
    return keyScores[0].first;
}

std::string CaesarBreaker::encrypt(const std::string& plaintext, int key) {
    // Encryption is the reverse of decryption
    return caesarShift(plaintext, 26 - key);
}

std::map<int, double> CaesarBreaker::getDetailedAnalysis(const std::string& ciphertext) {
    findPossibleKeys(ciphertext); // This populates lastAnalysis
    return lastAnalysis;
}

char CaesarBreaker::caesarShiftChar(char c, int shift) {
    if (!std::isalpha(c)) {
        return c;
    }
    
    char base = std::isupper(c) ? 'A' : 'a';
    return static_cast<char>((c - base + shift) % 26 + base);
}

std::string CaesarBreaker::caesarShift(const std::string& text, int shift) {
    std::string result;
    result.reserve(text.length());
    
    for (char c : text) {
        result += caesarShiftChar(c, shift);
    }
    
    return result;
}

double CaesarBreaker::scoreText(const std::string& text) {
    if (static_cast<int>(text.length()) < minTextLength) {
        return 0.0; // Too short for reliable analysis
    }
    
    double chiScore = calculateChiSquared(text);
    double icScore = calculateICScore(text);
    double patternScore = checkCommonPatterns(text);
    
    // Combine scores with weights
    double combinedScore = 0.0;
    combinedScore += normalizeScore(1.0 / (1.0 + chiScore), 0.0, 1.0) * 0.5; // Chi-squared (inverted)
    combinedScore += icScore * 0.3; // Index of Coincidence
    combinedScore += patternScore * 0.2; // Common patterns
    
    return combinedScore;
}

double CaesarBreaker::calculateChiSquared(const std::string& text) {
    auto textFreq = analyzer.calculateFrequency(text);
    auto englishFreq = analyzer.getLanguageFrequencies(targetLanguage);
    
    if (englishFreq.empty()) {
        return 1000.0; // High penalty if no reference frequency
    }
    
    return analyzer.chiSquaredTest(textFreq, englishFreq);
}

double CaesarBreaker::calculateICScore(const std::string& text) {
    double ic = analyzer.calculateIndexOfCoincidence(text);
    
    // English text typically has IC around 0.067
    // Score based on how close we are to this value
    double englishIC = 0.067;
    double diff = std::abs(ic - englishIC);
    
    // Convert to 0-1 score (closer to English IC = higher score)
    return std::max(0.0, 1.0 - (diff * 10.0));
}

double CaesarBreaker::checkCommonPatterns(const std::string& text) {
    double score = 0.0;
    std::string normalizedText = Utils::normalizeText(text);
    
    // Check for common English patterns
    std::vector<std::string> commonWords = {
        "THE", "AND", "FOR", "ARE", "BUT", "NOT", "YOU", "ALL", "CAN", "HER",
        "WAS", "ONE", "OUR", "OUT", "DAY", "GET", "HAS", "HIM", "HIS", "HOW",
        "ITS", "MAY", "NEW", "NOW", "OLD", "SEE", "TWO", "WHO", "BOY", "DID",
        "MAN", "OWN", "SAY", "SHE", "TOO", "USE", "THAT", "WITH", "FROM",
        "HAVE", "THIS", "WILL", "WHAT", "WHEN", "WHERE", "WHICH", "THERE"
    };
    
    int foundWords = 0;
    for (const std::string& word : commonWords) {
        if (normalizedText.find(word) != std::string::npos) {
            foundWords++;
        }
    }
    
    // Score based on percentage of common words found
    score = static_cast<double>(foundWords) / commonWords.size();
    
    // Check for common bigrams
    auto bigrams = analyzer.findCommonBigrams(text, 10);
    std::vector<std::string> commonBigrams = {"TH", "HE", "IN", "ER", "AN", "RE", "ED", "ND", "ON", "EN"};
    
    int foundBigrams = 0;
    for (const auto& bigram : bigrams) {
        if (std::find(commonBigrams.begin(), commonBigrams.end(), bigram.first) != commonBigrams.end()) {
            foundBigrams++;
        }
    }
    
    // Add bigram score
    score += static_cast<double>(foundBigrams) / commonBigrams.size() * 0.5;
    
    return std::min(1.0, score); // Cap at 1.0
}

double CaesarBreaker::normalizeScore(double score, double minVal, double maxVal) {
    if (maxVal <= minVal) {
        return 0.0;
    }
    
    return std::max(0.0, std::min(1.0, (score - minVal) / (maxVal - minVal)));
}

void CaesarBreaker::updateProgress(int key, double score, const std::string& preview) {
    std::cout << "Key " << std::setw(2) << key 
              << " | Score: " << std::setw(6) << std::fixed << std::setprecision(3) << score
              << " | Preview: " << preview;
    
    if (preview.length() < 30) {
        std::cout << "...";
    }
    std::cout << std::endl;
}