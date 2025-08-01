#include "FrequencyAnalyzer.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

FrequencyAnalyzer::FrequencyAnalyzer() {
    initializeDefaultFrequencies();
    initializeBigramFrequencies();
    initializeTrigramFrequencies();
}

std::map<char, double> FrequencyAnalyzer::calculateFrequency(const std::string& text) {
    std::map<char, int> counts;
    int totalAlphaChars = 0;
    
    // Count each character
    for (char c : text) {
        if (std::isalpha(c)) {
            char upperC = std::toupper(c);
            counts[upperC]++;
            totalAlphaChars++;
        }
    }
    
    // Convert counts to percentages
    return normalizeFrequencies(counts, totalAlphaChars);
}

double FrequencyAnalyzer::chiSquaredTest(const std::map<char, double>& observed,
                                        const std::map<char, double>& expected) {
    double chiSquared = 0.0;
    
    for (char c = 'A'; c <= 'Z'; c++) {
        double obs = 0.0;
        double exp = 0.0;
        
        auto obsIt = observed.find(c);
        auto expIt = expected.find(c);
        
        if (obsIt != observed.end()) obs = obsIt->second;
        if (expIt != expected.end()) exp = expIt->second;
        
        // Avoid division by zero
        if (exp > 0.0) {
            double diff = obs - exp;
            chiSquared += (diff * diff) / exp;
        }
    }
    
    return chiSquared;
}

std::string FrequencyAnalyzer::detectLanguage(const std::string& text) {
    std::map<char, double> textFreq = calculateFrequency(text);
    
    double bestScore = std::numeric_limits<double>::max();
    std::string bestLanguage = "unknown";
    
    for (const auto& langPair : languageFrequencies) {
        double score = chiSquaredTest(textFreq, langPair.second);
        if (score < bestScore) {
            bestScore = score;
            bestLanguage = langPair.first;
        }
    }
    
    return bestLanguage;
}

bool FrequencyAnalyzer::loadLanguageFrequencies(const std::string& language,
                                               const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open frequency file " << filename << std::endl;
        return false;
    }
    
    std::map<char, double> frequencies;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip comments
        
        std::vector<std::string> parts = Utils::split(line, ',');
        if (parts.size() >= 2) {
            char character = std::toupper(parts[0][0]);
            double frequency = std::stod(parts[1]);
            frequencies[character] = frequency;
        }
    }
    
    if (!frequencies.empty()) {
        languageFrequencies[language] = frequencies;
        return true;
    }
    
    return false;
}

std::map<char, double> FrequencyAnalyzer::getLanguageFrequencies(const std::string& language) const {
    auto it = languageFrequencies.find(language);
    if (it != languageFrequencies.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::string> FrequencyAnalyzer::getSupportedLanguages() const {
    std::vector<std::string> languages;
    for (const auto& pair : languageFrequencies) {
        languages.push_back(pair.first);
    }
    return languages;
}

double FrequencyAnalyzer::calculateIndexOfCoincidence(const std::string& text) {
    std::map<char, int> counts;
    int totalAlphaChars = 0;
    
    // Count each character
    for (char c : text) {
        if (std::isalpha(c)) {
            char upperC = std::toupper(c);
            counts[upperC]++;
            totalAlphaChars++;
        }
    }
    
    if (totalAlphaChars <= 1) return 0.0;
    
    // Calculate IC = sum(ni(ni-1)) / (N(N-1))
    double ic = 0.0;
    for (const auto& pair : counts) {
        int ni = pair.second;
        ic += ni * (ni - 1);
    }
    
    ic /= (totalAlphaChars * (totalAlphaChars - 1));
    return ic;
}

std::vector<std::pair<std::string, double>> FrequencyAnalyzer::findCommonBigrams(const std::string& text, int count) {
    std::map<std::string, int> bigramCounts;
    std::string normalizedText = Utils::normalizeText(text);
    
    // Count bigrams
    for (size_t i = 0; i < normalizedText.length() - 1; i++) {
        std::string bigram = normalizedText.substr(i, 2);
        bigramCounts[bigram]++;
    }
    
    // Convert to percentages and sort
    std::vector<std::pair<std::string, double>> bigrams;
    int totalBigrams = normalizedText.length() - 1;
    
    for (const auto& pair : bigramCounts) {
        double frequency = (static_cast<double>(pair.second) / totalBigrams) * 100.0;
        bigrams.emplace_back(pair.first, frequency);
    }
    
    // Sort by frequency (descending)
    std::sort(bigrams.begin(), bigrams.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top 'count' bigrams
    if (bigrams.size() > static_cast<size_t>(count)) {
        bigrams.resize(count);
    }
    
    return bigrams;
}

std::vector<std::pair<std::string, double>> FrequencyAnalyzer::findCommonTrigrams(const std::string& text, int count) {
    std::map<std::string, int> trigramCounts;
    std::string normalizedText = Utils::normalizeText(text);
    
    // Count trigrams
    for (size_t i = 0; i < normalizedText.length() - 2; i++) {
        std::string trigram = normalizedText.substr(i, 3);
        trigramCounts[trigram]++;
    }
    
    // Convert to percentages and sort
    std::vector<std::pair<std::string, double>> trigrams;
    int totalTrigrams = normalizedText.length() - 2;
    
    for (const auto& pair : trigramCounts) {
        double frequency = (static_cast<double>(pair.second) / totalTrigrams) * 100.0;
        trigrams.emplace_back(pair.first, frequency);
    }
    
    // Sort by frequency (descending)
    std::sort(trigrams.begin(), trigrams.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top 'count' trigrams
    if (trigrams.size() > static_cast<size_t>(count)) {
        trigrams.resize(count);
    }
    
    return trigrams;
}

double FrequencyAnalyzer::scoreEnglishness(const std::string& text) {
    auto textFreq = calculateFrequency(text);
    auto englishFreq = getLanguageFrequencies("english");
    
    if (englishFreq.empty()) {
        return 0.0;
    }
    
    // Lower chi-squared = better match, so invert the score
    double chiSq = chiSquaredTest(textFreq, englishFreq);
    return 1.0 / (1.0 + chiSq);
}

void FrequencyAnalyzer::initializeDefaultFrequencies() {
    // English letter frequencies (%)
    languageFrequencies["english"] = {
        {'A', 8.12}, {'B', 1.49}, {'C', 2.78}, {'D', 4.25}, {'E', 12.02},
        {'F', 2.23}, {'G', 2.02}, {'H', 6.09}, {'I', 6.97}, {'J', 0.15},
        {'K', 0.77}, {'L', 4.03}, {'M', 2.41}, {'N', 6.75}, {'O', 7.51},
        {'P', 1.93}, {'Q', 0.10}, {'R', 5.99}, {'S', 6.33}, {'T', 9.06},
        {'U', 2.76}, {'V', 0.98}, {'W', 2.36}, {'X', 0.15}, {'Y', 1.97},
        {'Z', 0.07}
    };
    
    // Turkish letter frequencies (%)
    languageFrequencies["turkish"] = {
        {'A', 11.92}, {'B', 2.65}, {'C', 0.96}, {'D', 4.87}, {'E', 8.91},
        {'F', 0.41}, {'G', 1.24}, {'H', 1.16}, {'I', 8.60}, {'J', 0.00},
        {'K', 4.68}, {'L', 5.92}, {'M', 3.75}, {'N', 7.23}, {'O', 2.72},
        {'P', 0.84}, {'Q', 0.00}, {'R', 6.92}, {'S', 3.01}, {'T', 5.71},
        {'U', 3.39}, {'V', 0.95}, {'W', 0.00}, {'X', 0.00}, {'Y', 3.34},
        {'Z', 1.52}
    };
}

void FrequencyAnalyzer::initializeBigramFrequencies() {
    // Common English bigrams with their frequencies
    englishBigrams = {
        {"TH", 2.71}, {"HE", 2.33}, {"IN", 2.03}, {"ER", 1.78}, {"AN", 1.61},
        {"RE", 1.41}, {"ED", 1.17}, {"ND", 1.07}, {"ON", 1.06}, {"EN", 1.05},
        {"AT", 1.03}, {"OU", 1.02}, {"IT", 1.00}, {"IS", 0.98}, {"OR", 0.91},
        {"TI", 0.89}, {"AS", 0.87}, {"TE", 0.87}, {"ET", 0.76}, {"NG", 0.76}
    };
}

void FrequencyAnalyzer::initializeTrigramFrequencies() {
    // Common English trigrams
    englishTrigrams = {
        {"THE", 1.81}, {"AND", 0.73}, {"ING", 0.72}, {"HER", 0.36}, {"HAT", 0.31},
        {"HIS", 0.31}, {"THA", 0.31}, {"ERE", 0.31}, {"FOR", 0.28}, {"ENT", 0.28},
        {"ION", 0.27}, {"TER", 0.24}, {"HAS", 0.24}, {"YOU", 0.24}, {"ITH", 0.23},
        {"VER", 0.22}, {"ALL", 0.22}, {"WIT", 0.21}, {"THI", 0.21}, {"TIO", 0.21}
    };
}

std::map<char, double> FrequencyAnalyzer::normalizeFrequencies(const std::map<char, int>& frequencies, 
                                                             int totalChars) const {
    std::map<char, double> normalized;
    
    if (totalChars == 0) return normalized;
    
    for (const auto& pair : frequencies) {
        normalized[pair.first] = (static_cast<double>(pair.second) / totalChars) * 100.0;
    }
    
    return normalized;
}