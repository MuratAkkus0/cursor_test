#include "AutoCipherDetector.h"
#include "Utils.h"
#include <algorithm>
#include <cmath>
#include <sstream>

AutoCipherDetector::AutoCipherDetector() : minTextLength(50) {
    // Initialize with reasonable defaults
}

AutoCipherDetector::CipherDetectionResult AutoCipherDetector::detectCipherType(const std::string& ciphertext) {
    if (ciphertext.length() < static_cast<size_t>(minTextLength)) {
        return CipherDetectionResult("unknown", 0.0, 
            "Text too short for reliable cipher detection (minimum " + 
            std::to_string(minTextLength) + " characters)");
    }
    
    // Calculate scores for each cipher type
    auto allScores = getAllCipherScores(ciphertext);
    
    // Find the highest scoring cipher type
    std::string bestCipher = "unknown";
    double bestScore = 0.0;
    
    for (const auto& pair : allScores) {
        if (pair.second > bestScore) {
            bestScore = pair.second;
            bestCipher = pair.first;
        }
    }
    
    // Generate reasoning
    auto analysis = analyzeFrequencyPatterns(ciphertext);
    std::string reasoning = generateReasoning(bestCipher, analysis);
    
    CipherDetectionResult result(bestCipher, bestScore, reasoning);
    result.scores = allScores;
    
    return result;
}

std::map<std::string, double> AutoCipherDetector::getAllCipherScores(const std::string& ciphertext) {
    std::string normalizedText = Utils::normalizeText(ciphertext);
    
    std::map<std::string, double> scores;
    
    scores["plaintext"] = calculatePlaintextScore(normalizedText);
    scores["caesar"] = calculateCaesarScore(normalizedText);
    scores["substitution"] = calculateSubstitutionScore(normalizedText);
    scores["vigenere"] = calculateVigenereScore(normalizedText);
    
    return scores;
}

double AutoCipherDetector::calculatePlaintextScore(const std::string& text) {
    // Check if text looks like normal English
    double englishnessScore = frequencyAnalyzer.scoreEnglishness(text);
    
    // Check Index of Coincidence (English ~0.067)
    double ic = frequencyAnalyzer.calculateIndexOfCoincidence(text);
    double icScore = 1.0 - std::abs(ic - 0.067) * 10.0; // Closer to 0.067 = higher score
    icScore = std::max(0.0, icScore);
    
    // Check for common English patterns
    auto bigrams = frequencyAnalyzer.findCommonBigrams(text, 5);
    double bigramScore = 0.0;
    std::vector<std::string> commonBigrams = {"TH", "HE", "IN", "ER", "AN"};
    
    for (const auto& bigram : bigrams) {
        if (std::find(commonBigrams.begin(), commonBigrams.end(), bigram.first) != commonBigrams.end()) {
            bigramScore += 0.2; // Each common bigram adds 0.2
        }
    }
    bigramScore = std::min(1.0, bigramScore);
    
    // Combined score
    return (englishnessScore * 0.5 + icScore * 0.3 + bigramScore * 0.2);
}

double AutoCipherDetector::calculateCaesarScore(const std::string& text) {
    // Caesar cipher characteristics:
    // 1. Maintains letter frequency distribution shape (just shifted)
    // 2. High IC value (similar to plaintext)
    // 3. Can be broken easily with frequency analysis
    
    double ic = frequencyAnalyzer.calculateIndexOfCoincidence(text);
    
    // Caesar should have high IC (similar to plaintext)
    double icScore = ic > 0.060 ? 1.0 : ic / 0.060;
    
    // Check if frequency distribution looks "natural" (not too flat)
    auto frequencies = frequencyAnalyzer.calculateFrequency(text);
    double maxFreq = 0.0, minFreq = 100.0;
    for (const auto& pair : frequencies) {
        maxFreq = std::max(maxFreq, pair.second);
        minFreq = std::min(minFreq, pair.second);
    }
    
    double freqVariation = maxFreq - minFreq;
    double variationScore = freqVariation > 5.0 ? 1.0 : freqVariation / 5.0;
    
    // Check if it's NOT plaintext (lower englishness)
    double englishnessScore = frequencyAnalyzer.scoreEnglishness(text);
    double cipherScore = englishnessScore < 0.3 ? 1.0 : (0.3 - englishnessScore) / 0.3;
    cipherScore = std::max(0.0, cipherScore);
    
    return (icScore * 0.4 + variationScore * 0.3 + cipherScore * 0.3);
}

double AutoCipherDetector::calculateSubstitutionScore(const std::string& text) {
    // Substitution cipher characteristics:
    // 1. High IC (monoalphabetic)
    // 2. Frequency distribution shape preserved but letters scrambled
    // 3. No obvious repeating patterns
    
    double ic = frequencyAnalyzer.calculateIndexOfCoincidence(text);
    
    // Substitution should have high IC
    double icScore = ic > 0.060 ? 1.0 : ic / 0.060;
    
    // Check frequency distribution variance (should be high)
    auto frequencies = frequencyAnalyzer.calculateFrequency(text);
    std::vector<double> freqValues;
    for (const auto& pair : frequencies) {
        freqValues.push_back(pair.second);
    }
    
    double mean = 0.0;
    for (double freq : freqValues) {
        mean += freq;
    }
    mean /= freqValues.size();
    
    double variance = 0.0;
    for (double freq : freqValues) {
        variance += (freq - mean) * (freq - mean);
    }
    variance /= freqValues.size();
    
    double varianceScore = variance > 10.0 ? 1.0 : variance / 10.0;
    
    // Check if it's NOT plaintext
    double englishnessScore = frequencyAnalyzer.scoreEnglishness(text);
    double cipherScore = englishnessScore < 0.4 ? 1.0 : (0.4 - englishnessScore) / 0.4;
    cipherScore = std::max(0.0, cipherScore);
    
    return (icScore * 0.4 + varianceScore * 0.3 + cipherScore * 0.3);
}

double AutoCipherDetector::calculateVigenereScore(const std::string& text) {
    // Vigenère cipher characteristics:
    // 1. Lower IC than monoalphabetic ciphers
    // 2. IC varies with key length
    // 3. Repeating patterns may exist
    
    double ic = frequencyAnalyzer.calculateIndexOfCoincidence(text);
    
    // Vigenère should have lower IC (around 0.04-0.05 for English)
    double icScore = 0.0;
    if (ic >= 0.038 && ic <= 0.055) {
        icScore = 1.0; // Perfect range for Vigenère
    } else if (ic < 0.038) {
        icScore = ic / 0.038; // Too low
    } else {
        icScore = std::max(0.0, 1.0 - (ic - 0.055) / 0.020); // Too high
    }
    
    // Check for repeating patterns (Kasiski test indicator)
    int repeatCount = 0;
    for (int len = 3; len <= 6 && len < static_cast<int>(text.length()) / 3; len++) {
        for (size_t i = 0; i <= text.length() - len; i++) {
            std::string pattern = text.substr(i, len);
            size_t nextPos = text.find(pattern, i + len);
            if (nextPos != std::string::npos) {
                repeatCount++;
            }
        }
    }
    
    double patternScore = repeatCount > 2 ? 1.0 : static_cast<double>(repeatCount) / 2.0;
    
    // Check if it's NOT plaintext
    double englishnessScore = frequencyAnalyzer.scoreEnglishness(text);
    double cipherScore = englishnessScore < 0.3 ? 1.0 : (0.3 - englishnessScore) / 0.3;
    cipherScore = std::max(0.0, cipherScore);
    
    return (icScore * 0.5 + patternScore * 0.2 + cipherScore * 0.3);
}

std::map<std::string, double> AutoCipherDetector::analyzeFrequencyPatterns(const std::string& text) {
    std::map<std::string, double> analysis;
    
    // Basic statistics
    analysis["ic"] = frequencyAnalyzer.calculateIndexOfCoincidence(text);
    analysis["englishness"] = frequencyAnalyzer.scoreEnglishness(text);
    
    // Frequency distribution analysis
    auto frequencies = frequencyAnalyzer.calculateFrequency(text);
    double maxFreq = 0.0, minFreq = 100.0, totalFreq = 0.0;
    for (const auto& pair : frequencies) {
        maxFreq = std::max(maxFreq, pair.second);
        minFreq = std::min(minFreq, pair.second);
        totalFreq += pair.second;
    }
    
    analysis["freq_max"] = maxFreq;
    analysis["freq_min"] = minFreq;
    analysis["freq_range"] = maxFreq - minFreq;
    analysis["freq_variance"] = 0.0; // Calculate variance
    
    double mean = totalFreq / frequencies.size();
    for (const auto& pair : frequencies) {
        analysis["freq_variance"] += (pair.second - mean) * (pair.second - mean);
    }
    analysis["freq_variance"] /= frequencies.size();
    
    // Pattern analysis
    auto bigrams = frequencyAnalyzer.findCommonBigrams(text, 10);
    analysis["common_bigrams"] = static_cast<double>(bigrams.size());
    
    return analysis;
}

std::string AutoCipherDetector::generateReasoning(const std::string& cipherType, 
                                                 const std::map<std::string, double>& analysis) {
    std::stringstream reasoning;
    
    double ic = analysis.at("ic");
    double englishness = analysis.at("englishness");
    double freqRange = analysis.at("freq_range");
    
    reasoning << "Analysis suggests " << cipherType << " cipher. ";
    
    if (cipherType == "plaintext") {
        reasoning << "High englishness score (" << std::fixed << std::setprecision(3) << englishness 
                  << ") and typical IC (" << ic << ") indicate normal English text.";
    }
    else if (cipherType == "caesar") {
        reasoning << "High IC (" << ic << ") suggests monoalphabetic cipher. "
                  << "Frequency range (" << freqRange << ") indicates preserved distribution. "
                  << "Low englishness (" << englishness << ") confirms encryption.";
    }
    else if (cipherType == "substitution") {
        reasoning << "High IC (" << ic << ") indicates monoalphabetic substitution. "
                  << "Wide frequency range (" << freqRange << ") shows natural language patterns. "
                  << "Low englishness (" << englishness << ") confirms complex substitution.";
    }
    else if (cipherType == "vigenere") {
        reasoning << "Lower IC (" << ic << ") suggests polyalphabetic cipher. "
                  << "Reduced frequency variance indicates key-based encryption. "
                  << "Pattern analysis supports Vigenère characteristics.";
    }
    else {
        reasoning << "Insufficient data or unclear patterns for reliable detection. "
                  << "IC: " << ic << ", Englishness: " << englishness;
    }
    
    return reasoning.str();
}