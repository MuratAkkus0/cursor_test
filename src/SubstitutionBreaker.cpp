#include "SubstitutionBreaker.h"
#include "Utils.h"
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>

SubstitutionBreaker::SubstitutionBreaker() 
    : optimizationMethod("hybrid"), maxIterations(1000), rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    initializeBigramFrequencies();
    initializeTrigramFrequencies();
    loadCommonWords();
}

std::string SubstitutionBreaker::breakCipher(const std::string& ciphertext) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!validateInput(ciphertext)) {
        return "";
    }
    
    printVerbose("Starting Substitution cipher analysis...");
    printVerbose("Text length: " + std::to_string(ciphertext.length()));
    printVerbose("Optimization method: " + optimizationMethod);
    
    optimizationHistory.clear();
    
    std::map<char, char> bestMapping;
    
    if (optimizationMethod == "frequency") {
        bestMapping = generateMapping(ciphertext);
    } else if (optimizationMethod == "hill_climbing") {
        auto initialMapping = generateMapping(ciphertext);
        bestMapping = hillClimbOptimization(initialMapping, ciphertext);
    } else if (optimizationMethod == "simulated_annealing") {
        auto initialMapping = generateMapping(ciphertext);
        bestMapping = simulatedAnnealing(initialMapping, ciphertext);
    } else { // hybrid (default)
        // Start with frequency analysis
        auto initialMapping = generateMapping(ciphertext);
        // Improve with bigrams
        auto bigramMapping = improveMappingWithBigrams(initialMapping, ciphertext);
        // Final optimization with hill climbing
        bestMapping = hillClimbOptimization(bigramMapping, ciphertext, maxIterations / 2);
    }
    
    std::string result = applyMapping(ciphertext, bestMapping);
    
    // Calculate final score and confidence
    double finalScore = scoreMapping(bestMapping, ciphertext);
    updateConfidence(finalScore, 1.0); // Normalize to 0-1 range
    
    // Calculate analysis time
    auto end = std::chrono::high_resolution_clock::now();
    analysisTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    
    printVerbose("Analysis completed in " + std::to_string(analysisTimeMs) + " ms");
    printVerbose("Final score: " + std::to_string(finalScore));
    printVerbose("Confidence: " + std::to_string(confidence) + "%");
    
    return result;
}

std::vector<std::string> SubstitutionBreaker::getPossibleSolutions(const std::string& ciphertext) {
    if (!validateInput(ciphertext)) {
        return {};
    }
    
    std::vector<std::string> solutions;
    
    // Generate initial mapping
    auto initialMapping = generateMapping(ciphertext);
    solutions.push_back(applyMapping(ciphertext, initialMapping));
    
    // Try different optimization approaches for variety
    auto bigramMapping = improveMappingWithBigrams(initialMapping, ciphertext);
    solutions.push_back(applyMapping(ciphertext, bigramMapping));
    
    auto hillClimbMapping = hillClimbOptimization(initialMapping, ciphertext, 500);
    solutions.push_back(applyMapping(ciphertext, hillClimbMapping));
    
    // Remove duplicates
    std::sort(solutions.begin(), solutions.end());
    solutions.erase(std::unique(solutions.begin(), solutions.end()), solutions.end());
    
    return solutions;
}

double SubstitutionBreaker::scorePlaintext(const std::string& plaintext) {
    double freqScore = analyzer.scoreEnglishness(plaintext);
    double bigramScore = calculateBigramScore(plaintext);
    double trigramScore = calculateTrigramScore(plaintext);
    double wordScore = calculateWordScore(plaintext);
    
    return calculateCombinedScore(freqScore, bigramScore, trigramScore, wordScore);
}

std::string SubstitutionBreaker::breakByFrequency(const std::string& ciphertext) {
    auto mapping = generateMapping(ciphertext);
    return applyMapping(ciphertext, mapping);
}

std::map<char, char> SubstitutionBreaker::generateMapping(const std::string& ciphertext) {
    printVerbose("Generating initial frequency-based mapping...");
    
    std::string normalizedText = Utils::normalizeText(ciphertext);
    
    // Get cipher character frequencies
    auto cipherFreq = analyzer.calculateFrequency(normalizedText);
    auto englishFreq = analyzer.getLanguageFrequencies(targetLanguage);
    
    // Sort characters by frequency
    std::vector<std::pair<char, double>> cipherSorted;
    std::vector<std::pair<char, double>> englishSorted;
    
    for (const auto& pair : cipherFreq) {
        cipherSorted.emplace_back(pair.first, pair.second);
    }
    
    for (const auto& pair : englishFreq) {
        englishSorted.emplace_back(pair.first, pair.second);
    }
    
    // Sort by frequency (descending)
    std::sort(cipherSorted.begin(), cipherSorted.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    std::sort(englishSorted.begin(), englishSorted.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Create mapping by matching frequencies
    std::map<char, char> mapping;
    size_t minSize = std::min(cipherSorted.size(), englishSorted.size());
    
    for (size_t i = 0; i < minSize; i++) {
        mapping[cipherSorted[i].first] = englishSorted[i].first;
    }
    
    // Complete the mapping for any missing characters
    mapping = completeMapping(mapping);
    
    printVerbose("Initial mapping generated with " + std::to_string(mapping.size()) + " characters");
    
    return mapping;
}

std::map<char, char> SubstitutionBreaker::improveMappingWithBigrams(const std::map<char, char>& initialMapping,
                                                                    const std::string& ciphertext) {
    printVerbose("Improving mapping with bigram analysis...");
    
    auto mapping = initialMapping;
    std::string normalizedText = Utils::normalizeText(ciphertext);
    
    // Find most common bigrams in ciphertext
    auto cipherBigrams = analyzer.findCommonBigrams(normalizedText, 10);
    
    // Try to match with common English bigrams
    std::vector<std::string> commonEnglishBigrams = {"TH", "HE", "IN", "ER", "AN", "RE", "ED", "ND", "ON", "EN"};
    
    for (size_t i = 0; i < std::min(cipherBigrams.size(), commonEnglishBigrams.size()); i++) {
        std::string cipherBigram = cipherBigrams[i].first;
        std::string englishBigram = commonEnglishBigrams[i];
        
        if (cipherBigram.length() == 2 && englishBigram.length() == 2) {
            // Suggest mapping for these bigram characters
            char cipher1 = cipherBigram[0];
            char cipher2 = cipherBigram[1];
            char english1 = englishBigram[0];
            char english2 = englishBigram[1];
            
            // Only update if it improves the score
            auto testMapping = mapping;
            testMapping[cipher1] = english1;
            testMapping[cipher2] = english2;
            
            if (scoreMapping(testMapping, ciphertext) > scoreMapping(mapping, ciphertext)) {
                mapping[cipher1] = english1;
                mapping[cipher2] = english2;
            }
        }
    }
    
    printVerbose("Bigram analysis completed");
    return mapping;
}

double SubstitutionBreaker::scoreMapping(const std::map<char, char>& mapping, const std::string& ciphertext) {
    std::string decrypted = applyMapping(ciphertext, mapping);
    return scorePlaintext(decrypted);
}

std::string SubstitutionBreaker::applyMapping(const std::string& text, const std::map<char, char>& mapping) {
    std::string result;
    result.reserve(text.length());
    
    for (char c : text) {
        if (std::isalpha(c)) {
            char upperC = std::toupper(c);
            auto it = mapping.find(upperC);
            if (it != mapping.end()) {
                char mapped = it->second;
                result += std::isupper(c) ? mapped : std::tolower(mapped);
            } else {
                result += c; // Keep unmapped characters as-is
            }
        } else {
            result += c; // Keep non-alphabetic characters
        }
    }
    
    return result;
}

double SubstitutionBreaker::calculateBigramScore(const std::string& text) {
    std::string normalizedText = Utils::normalizeText(text);
    double score = 0.0;
    int totalBigrams = 0;
    
    for (size_t i = 0; i < normalizedText.length() - 1; i++) {
        std::string bigram = normalizedText.substr(i, 2);
        auto it = bigramFreq.find(bigram);
        if (it != bigramFreq.end()) {
            score += it->second;
        }
        totalBigrams++;
    }
    
    return totalBigrams > 0 ? score / totalBigrams : 0.0;
}

double SubstitutionBreaker::calculateTrigramScore(const std::string& text) {
    std::string normalizedText = Utils::normalizeText(text);
    double score = 0.0;
    int totalTrigrams = 0;
    
    for (size_t i = 0; i < normalizedText.length() - 2; i++) {
        std::string trigram = normalizedText.substr(i, 3);
        auto it = trigramFreq.find(trigram);
        if (it != trigramFreq.end()) {
            score += it->second;
        }
        totalTrigrams++;
    }
    
    return totalTrigrams > 0 ? score / totalTrigrams : 0.0;
}

double SubstitutionBreaker::calculateWordScore(const std::string& text) {
    std::string normalizedText = Utils::normalizeText(text);
    int foundWords = 0;
    int totalWords = 0;
    
    // Simple word extraction (split by spaces in original text)
    std::vector<std::string> words = Utils::split(text, ' ');
    
    for (const std::string& word : words) {
        std::string normalizedWord = Utils::normalizeText(word);
        if (normalizedWord.length() >= 3) { // Only consider words of 3+ characters
            totalWords++;
            if (commonWords.find(normalizedWord) != commonWords.end()) {
                foundWords++;
            }
        }
    }
    
    return totalWords > 0 ? static_cast<double>(foundWords) / totalWords : 0.0;
}

std::map<char, char> SubstitutionBreaker::completeMapping(const std::map<char, char>& mapping) {
    auto result = mapping;
    
    std::set<char> usedPlainChars;
    for (const auto& pair : mapping) {
        usedPlainChars.insert(pair.second);
    }
    
    // Find unused characters
    std::vector<char> unusedCipherChars;
    std::vector<char> unusedPlainChars;
    
    for (char c = 'A'; c <= 'Z'; c++) {
        if (mapping.find(c) == mapping.end()) {
            unusedCipherChars.push_back(c);
        }
        if (usedPlainChars.find(c) == usedPlainChars.end()) {
            unusedPlainChars.push_back(c);
        }
    }
    
    // Map remaining characters
    size_t minSize = std::min(unusedCipherChars.size(), unusedPlainChars.size());
    for (size_t i = 0; i < minSize; i++) {
        result[unusedCipherChars[i]] = unusedPlainChars[i];
    }
    
    return result;
}

double SubstitutionBreaker::calculateCombinedScore(double freqScore, double bigramScore, 
                                                  double trigramScore, double wordScore) {
    // Weighted combination of scores
    return freqScore * 0.3 + bigramScore * 0.3 + trigramScore * 0.2 + wordScore * 0.2;
}

void SubstitutionBreaker::initializeBigramFrequencies() {
    // Common English bigrams with frequencies (normalized to 0-1)
    bigramFreq = {
        {"TH", 0.0271}, {"HE", 0.0233}, {"IN", 0.0203}, {"ER", 0.0178}, {"AN", 0.0161},
        {"RE", 0.0141}, {"ED", 0.0117}, {"ND", 0.0107}, {"ON", 0.0106}, {"EN", 0.0105},
        {"AT", 0.0103}, {"OU", 0.0102}, {"IT", 0.0100}, {"IS", 0.0098}, {"OR", 0.0091},
        {"TI", 0.0089}, {"AS", 0.0087}, {"TE", 0.0087}, {"ET", 0.0076}, {"NG", 0.0076},
        {"OF", 0.0075}, {"AL", 0.0074}, {"DE", 0.0070}, {"SE", 0.0068}, {"LE", 0.0066},
        {"SA", 0.0063}, {"SI", 0.0062}, {"AR", 0.0062}, {"VE", 0.0058}, {"RA", 0.0057},
        {"LD", 0.0057}, {"UR", 0.0056}, {"TA", 0.0056}, {"RI", 0.0055}, {"NE", 0.0055}
    };
}

void SubstitutionBreaker::initializeTrigramFrequencies() {
    // Common English trigrams with frequencies (normalized to 0-1)
    trigramFreq = {
        {"THE", 0.0181}, {"AND", 0.0073}, {"ING", 0.0072}, {"HER", 0.0036}, {"HAT", 0.0031},
        {"HIS", 0.0031}, {"THA", 0.0031}, {"ERE", 0.0031}, {"FOR", 0.0028}, {"ENT", 0.0028},
        {"ION", 0.0027}, {"TER", 0.0024}, {"HAS", 0.0024}, {"YOU", 0.0024}, {"ITH", 0.0023},
        {"VER", 0.0022}, {"ALL", 0.0022}, {"WIT", 0.0021}, {"THI", 0.0021}, {"TIO", 0.0021},
        {"EST", 0.0020}, {"ARE", 0.0019}, {"HEN", 0.0019}, {"RST", 0.0019}, {"OUR", 0.0018},
        {"OUT", 0.0018}, {"HAV", 0.0018}, {"ATE", 0.0017}, {"STH", 0.0017}, {"VED", 0.0017}
    };
}

void SubstitutionBreaker::loadCommonWords() {
    // Load common English words for validation
    commonWords = {
        "THE", "AND", "FOR", "ARE", "BUT", "NOT", "YOU", "ALL", "CAN", "HER",
        "WAS", "ONE", "OUR", "OUT", "DAY", "GET", "HAS", "HIM", "HIS", "HOW",
        "ITS", "MAY", "NEW", "NOW", "OLD", "SEE", "TWO", "WHO", "BOY", "DID",
        "MAN", "OWN", "SAY", "SHE", "TOO", "USE", "THAT", "WITH", "FROM", "THIS",
        "HAVE", "WILL", "WHAT", "WHEN", "WHERE", "WHICH", "THERE", "WOULD", "ABOUT",
        "AFTER", "FIRST", "NEVER", "THESE", "THINK", "WHERE", "BEING", "EVERY",
        "GREAT", "MIGHT", "SHALL", "STILL", "THOSE", "UNDER", "WHILE", "COULD"
    };
}

std::map<char, char> SubstitutionBreaker::hillClimbOptimization(const std::map<char, char>& initialMapping,
                                                               const std::string& ciphertext,
                                                               int maxIterations) {
    printVerbose("Starting hill climbing optimization...");
    
    auto currentMapping = initialMapping;
    double currentScore = scoreMapping(currentMapping, ciphertext);
    
    int iteration = 0;
    int improvementCount = 0;
    
    while (iteration < maxIterations) {
        // Generate neighbor mapping
        auto neighborMapping = generateNeighborMapping(currentMapping);
        double neighborScore = scoreMapping(neighborMapping, ciphertext);
        
        // Accept if better
        if (neighborScore > currentScore) {
            currentMapping = neighborMapping;
            currentScore = neighborScore;
            improvementCount++;
            
            updateOptimizationProgress(iteration, currentScore);
        }
        
        iteration++;
        
        // Log progress every 100 iterations
        if (verbose && iteration % 100 == 0) {
            printVerbose("Iteration " + std::to_string(iteration) + 
                        ", Score: " + std::to_string(currentScore) +
                        ", Improvements: " + std::to_string(improvementCount));
        }
    }
    
    printVerbose("Hill climbing completed. Final score: " + std::to_string(currentScore));
    printVerbose("Total improvements: " + std::to_string(improvementCount));
    
    return currentMapping;
}

std::map<char, char> SubstitutionBreaker::simulatedAnnealing(const std::map<char, char>& initialMapping,
                                                            const std::string& ciphertext,
                                                            int maxIterations,
                                                            double initialTemperature) {
    printVerbose("Starting simulated annealing optimization...");
    printVerbose("Initial temperature: " + std::to_string(initialTemperature));
    
    auto currentMapping = initialMapping;
    auto bestMapping = initialMapping;
    double currentScore = scoreMapping(currentMapping, ciphertext);
    double bestScore = currentScore;
    
    double temperature = initialTemperature;
    double coolingRate = std::pow(0.001 / initialTemperature, 1.0 / maxIterations);
    
    int acceptedMoves = 0;
    int rejectedMoves = 0;
    
    for (int iteration = 0; iteration < maxIterations; iteration++) {
        // Generate neighbor mapping
        auto neighborMapping = generateNeighborMapping(currentMapping);
        double neighborScore = scoreMapping(neighborMapping, ciphertext);
        
        // Calculate acceptance probability
        double deltaScore = neighborScore - currentScore;
        bool accept = false;
        
        if (deltaScore > 0) {
            // Better solution - always accept
            accept = true;
        } else {
            // Worse solution - accept with probability based on temperature
            double probability = std::exp(deltaScore / temperature);
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            accept = dist(rng) < probability;
        }
        
        if (accept) {
            currentMapping = neighborMapping;
            currentScore = neighborScore;
            acceptedMoves++;
            
            // Update best solution if this is better
            if (currentScore > bestScore) {
                bestMapping = currentMapping;
                bestScore = currentScore;
                
                updateOptimizationProgress(iteration, bestScore, temperature);
            }
        } else {
            rejectedMoves++;
        }
        
        // Cool down
        temperature *= coolingRate;
        
        // Log progress
        if (verbose && iteration % 200 == 0) {
            printVerbose("Iteration " + std::to_string(iteration) + 
                        ", Current: " + std::to_string(currentScore) +
                        ", Best: " + std::to_string(bestScore) +
                        ", Temp: " + std::to_string(temperature));
        }
    }
    
    printVerbose("Simulated annealing completed. Best score: " + std::to_string(bestScore));
    printVerbose("Accepted moves: " + std::to_string(acceptedMoves));
    printVerbose("Rejected moves: " + std::to_string(rejectedMoves));
    
    return bestMapping;
}

std::map<char, char> SubstitutionBreaker::generateNeighborMapping(const std::map<char, char>& mapping) {
    // Create a copy of the mapping
    auto neighbor = mapping;
    
    // Get all cipher characters
    std::vector<char> cipherChars;
    for (const auto& pair : mapping) {
        cipherChars.push_back(pair.first);
    }
    
    if (cipherChars.size() < 2) {
        return neighbor; // Cannot swap if less than 2 characters
    }
    
    // Randomly select two different cipher characters to swap their mappings
    std::uniform_int_distribution<size_t> dist(0, cipherChars.size() - 1);
    
    size_t idx1 = dist(rng);
    size_t idx2 = dist(rng);
    
    // Ensure we select two different characters
    while (idx1 == idx2 && cipherChars.size() > 1) {
        idx2 = dist(rng);
    }
    
    char char1 = cipherChars[idx1];
    char char2 = cipherChars[idx2];
    
    // Swap their mappings
    char temp = neighbor[char1];
    neighbor[char1] = neighbor[char2];
    neighbor[char2] = temp;
    
    return neighbor;
}

std::map<char, char> SubstitutionBreaker::swapCharacters(const std::map<char, char>& mapping, 
                                                        char char1, char char2) {
    auto result = mapping;
    
    auto it1 = result.find(char1);
    auto it2 = result.find(char2);
    
    if (it1 != result.end() && it2 != result.end()) {
        char temp = it1->second;
        it1->second = it2->second;
        it2->second = temp;
    }
    
    return result;
}

void SubstitutionBreaker::updateOptimizationProgress(int iteration, double score, double temperature) {
    optimizationHistory.emplace_back(iteration, score);
    
    if (verbose) {
        std::string message = "Optimization progress - Iteration: " + std::to_string(iteration) +
                             ", Score: " + std::to_string(score);
        if (temperature > 0) {
            message += ", Temperature: " + std::to_string(temperature);
        }
        printVerbose(message);
    }
}